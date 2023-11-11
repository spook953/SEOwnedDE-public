#include "AutoVaccinator.h"

#include "../CFG.h"

#include "../AimUtils/AimUtils.h"

int sim_res_type{ MEDIGUN_NUM_RESISTS };
int goal_res_type{ MEDIGUN_BULLET_RESIST };
bool changing_res{ false };
bool pop{ false };

medigun_resist_types_t weaponIDToResType(int weapon_id)
{
	switch (weapon_id)
	{
		case TF_WEAPON_SHOTGUN_PRIMARY:
		case TF_WEAPON_SHOTGUN_SOLDIER:
		case TF_WEAPON_SHOTGUN_HWG:
		case TF_WEAPON_SHOTGUN_PYRO:
		case TF_WEAPON_SCATTERGUN:
		case TF_WEAPON_SNIPERRIFLE:
		case TF_WEAPON_MINIGUN:
		case TF_WEAPON_SMG:
		case TF_WEAPON_PISTOL:
		case TF_WEAPON_PISTOL_SCOUT:
		case TF_WEAPON_REVOLVER:
		case TF_WEAPON_SENTRY_BULLET:
		case TF_WEAPON_COMPOUND_BOW:
		case TF_WEAPON_SENTRY_REVENGE:
		case TF_WEAPON_HANDGUN_SCOUT_PRIMARY:
		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_HANDGUN_SCOUT_SECONDARY:
		case TF_WEAPON_SODA_POPPER:
		case TF_WEAPON_SNIPERRIFLE_DECAP:
		case TF_WEAPON_PEP_BRAWLER_BLASTER:
		case TF_WEAPON_CHARGED_SMG:
		case TF_WEAPON_SNIPERRIFLE_CLASSIC:
		case TF_WEAPON_RAYGUN:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
		case TF_WEAPON_DRG_POMSON:
		case TF_WEAPON_SYRINGEGUN_MEDIC:
		case TF_WEAPON_LASER_POINTER:
		{
			return MEDIGUN_BULLET_RESIST;
		}

		case TF_WEAPON_ROCKETLAUNCHER:
		case TF_WEAPON_GRENADELAUNCHER:
		case TF_WEAPON_PIPEBOMBLAUNCHER:
		case TF_WEAPON_FLAMETHROWER_ROCKET:
		case TF_WEAPON_GRENADE_DEMOMAN:
		case TF_WEAPON_SENTRY_ROCKET:
		case TF_WEAPON_PUMPKIN_BOMB:
		case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
		case TF_WEAPON_CANNON:
		case TF_WEAPON_PARTICLE_CANNON:
		{
			return MEDIGUN_BLAST_RESIST;
		}

		case TF_WEAPON_FLAMETHROWER:
		case TF_WEAPON_FLAREGUN:
		case TF_WEAPON_FLAREGUN_REVENGE:
		case TF_WEAPON_FLAME_BALL:
		{
			return MEDIGUN_FIRE_RESIST;
		}

		default:
		{
			return MEDIGUN_NUM_RESISTS;
		}
	}
}

bool isPlayerInDanger(C_TFPlayer *player, medigun_resist_types_t &danger_type)
{
	if (!player || player->IsInvulnerable())
	{
		return false;
	}
	
	auto percent_health{ Math::RemapValClamped
	(
		static_cast<float>(player->m_iHealth()), 0.0f, static_cast<float>(player->GetMaxHealth()), 0.0f, 1.0f
	) };

	//I::DebugOverlay->ClearAllOverlays();

	for (auto ent : H::Entities->GetGroup(EEntGroup::PLAYERS_ENEMIES))
	{
		if (!ent)
		{
			continue;
		}

		auto enemy{ ent->As<C_TFPlayer>() };

		if (!enemy || enemy->deadflag())
		{
			continue;
		}

		if (enemy->m_iClass() != TF_CLASS_SNIPER)
		{
			continue;
		}

		auto weapon{ enemy->m_hActiveWeapon().Get()->As<C_TFWeaponBase>() };

		if (!weapon || weapon->GetSlot() != WEAPON_SLOT_PRIMARY || weapon->GetWeaponID() == TF_WEAPON_COMPOUND_BOW)
		{
			continue;
		}

		bool zoomed{ enemy->InCond(TF_COND_ZOOMED) };

		if (weapon->GetWeaponID() == TF_WEAPON_SNIPERRIFLE_CLASSIC)
		{
			zoomed = weapon->As<C_TFSniperRifleClassic>()->m_bCharging();
		}

		if (!enemy->IsCritBoosted() && !enemy->IsMiniCritBoosted() && !zoomed)
		{
			continue;
		}

		auto mins{ player->m_vecMins() };
		auto maxs{ player->m_vecMaxs() };

		mins.x *= 3.0f;
		mins.y *= 3.0f;
		mins.z *= 1.5f;

		maxs.x *= 3.0f;
		maxs.y *= 3.0f;
		maxs.z *= 1.5f;

		Vec3 forward{};

		Math::AngleVectors(enemy->GetEyeAngles(), &forward);

		if (!Math::RayToOBB(enemy->GetShootPos(), forward, player->m_vecOrigin(), mins, maxs, player->RenderableToWorldTransform()))
		{
			//I::DebugOverlay->AddSweptBoxOverlay(player->m_vecOrigin(), player->m_vecOrigin(), mins, maxs, {}, 255, 255, 255, 255, 0.1f);

			continue;
		}

		//I::DebugOverlay->AddSweptBoxOverlay(player->m_vecOrigin(), player->m_vecOrigin(), mins, maxs, {}, 0, 255, 0, 255, 0.1f);

		auto visible_from_center
		{ 
			F::AimUtils->TraceEntityAutoDet
			(
				ent,
				player->GetCenter(),
				enemy->m_vecOrigin() + Vec3{ 0.0f, 0.0f, enemy->m_vecMaxs().z }
			)
		};

		auto visible_from_head
		{ 
			F::AimUtils->TraceEntityAutoDet
			(
				ent,
				player->m_vecOrigin() + Vec3{ 0.0f, 0.0f, player->m_vecMaxs().z },
				enemy->m_vecOrigin() + Vec3{ 0.0f, 0.0f, enemy->m_vecMaxs().z }
			)
		};

		if (!visible_from_center && !visible_from_head)
		{
			continue;
		}

		danger_type = MEDIGUN_BULLET_RESIST;

		return true;
	}

	size_t num_close_pipebombs{};

	for (auto ent : H::Entities->GetGroup(EEntGroup::PROJECTILES_ENEMIES))
	{
		if (!ent)
		{
			continue;
		}

		auto visible_from_center{ F::AimUtils->TraceEntityAutoDet(ent, player->GetCenter(), ent->GetCenter()) };
		auto visible_from_head{ F::AimUtils->TraceEntityAutoDet(ent, player->m_vecOrigin() + Vec3{ 0.0f, 0.0f, player->m_vecMaxs().z }, ent->GetCenter())};

		if (!visible_from_center && !visible_from_head)
		{
			continue;
		}

		Vec3 vel{};

		ent->EstimateAbsVelocity(vel);

		auto ent_origin{ ent->m_vecOrigin() + (vel * SDKUtils::GetLatency()) };

		switch (ent->GetClassId())
		{
			case ETFClassIds::CTFProjectile_Arrow:
			{
				if (vel.IsZero() || ent_origin.DistTo(player->GetCenter()) > 150.0f)
				{
					continue;
				}

				danger_type = MEDIGUN_BULLET_RESIST;

				return true;
			}

			case ETFClassIds::CTFProjectile_HealingBolt:
			{
				if (vel.IsZero() || ent_origin.DistTo(player->GetCenter()) > 150.0f)
				{
					continue;
				}

				auto arrow{ ent->As<C_TFProjectile_Arrow>() };

				if (arrow->m_bCritical() || percent_health < 0.85f)
				{
					danger_type = MEDIGUN_BULLET_RESIST;

					return true;
				}

				break;
			}

			case ETFClassIds::CTFProjectile_Rocket:
			case ETFClassIds::CTFProjectile_SentryRocket:
			case ETFClassIds::CTFProjectile_EnergyBall:
			{
				if (ent_origin.DistTo(player->GetCenter()) > 250.0f)
				{
					continue;
				}

				auto rocket{ ent->As<C_TFProjectile_Rocket>() };

				if (rocket->m_bCritical() || percent_health < 1.0f)
				{
					danger_type = MEDIGUN_BLAST_RESIST;

					return true;
				}

				break;
			}

			case ETFClassIds::CTFGrenadePipebombProjectile:
			{
				if (ent_origin.DistTo(player->GetCenter()) > 250.0f)
				{
					continue;
				}

				auto bomb{ ent->As<C_TFGrenadePipebombProjectile>() };

				if (bomb->m_iType() == TF_GL_MODE_REMOTE_DETONATE_PRACTICE)
				{
					continue;
				}

				if (bomb->m_bCritical() || percent_health < 1.0f || ent_origin.DistTo(player->GetCenter()) < 100.0f)
				{
					danger_type = MEDIGUN_BLAST_RESIST;

					return true;
				}

				num_close_pipebombs++;

				break;
			}

			case ETFClassIds::CTFProjectile_Flare:
			{
				if (ent_origin.DistTo(player->GetCenter()) > 150.0f)
				{
					continue;
				}
				
				auto flare{ ent->As<C_TFProjectile_Flare>() };

				if (flare->m_bCritical() || player->InCond(TF_COND_BURNING) || player->InCond(TF_COND_BURNING_PYRO))
				{
					danger_type = MEDIGUN_FIRE_RESIST;

					return true;
				}

				break;
			}

			case ETFClassIds::CTFProjectile_BallOfFire:
			{
				if (ent_origin.DistTo(player->GetCenter()) > 150.0f)
				{
					continue;
				}

				if (player->InCond(TF_COND_BURNING) || player->InCond(TF_COND_BURNING_PYRO))
				{
					danger_type = MEDIGUN_FIRE_RESIST;

					return true;
				}

				break;
			}

			case ETFClassIds::CTFProjectile_EnergyRing:
			{
				if (ent_origin.DistTo(player->GetCenter()) > 150.0f)
				{
					continue;
				}

				if (percent_health < 0.85f)
				{
					danger_type = MEDIGUN_BULLET_RESIST;

					return true;
				}

				break;
			}

			default:
			{
				continue;
			}
		}
	}

	if (num_close_pipebombs > 0)
	{
		danger_type = MEDIGUN_BLAST_RESIST;
	}

	if (num_close_pipebombs > 1)
	{
		return true;
	}
	
	for (auto ent : H::Entities->GetGroup(EEntGroup::BUILDINGS_ENEMIES))
	{
		if (!ent || ent->GetClassId() != ETFClassIds::CObjectSentrygun)
		{
			continue;
		}

		C_ObjectSentrygun *sentrygun{ ent->As<C_ObjectSentrygun>() };

		if (!sentrygun)
		{
			continue;
		}

		if (sentrygun->m_hAutoAimTarget() == player || sentrygun->m_hEnemy() == player)
		{
			danger_type = MEDIGUN_BULLET_RESIST;

			if (percent_health < 0.85f)
			{
				return true;
			}
		}
	}

	return false;
}

bool playerHasResUber(medigun_resist_types_t res, C_TFPlayer *player)
{
	if (res == MEDIGUN_BULLET_RESIST)
	{
		return player->InCond(TF_COND_MEDIGUN_UBER_BULLET_RESIST);
	}

	if (res == MEDIGUN_BLAST_RESIST)
	{
		return player->InCond(TF_COND_MEDIGUN_UBER_BLAST_RESIST);
	}

	if (res == MEDIGUN_FIRE_RESIST)
	{
		return player->InCond(TF_COND_MEDIGUN_UBER_FIRE_RESIST);
	}

	return false;
}

void auto_vaccinator::run(CUserCmd *cmd)
{
	if (!CFG::AutoVaccinator_Active || !G::bCanPrimaryAttack)
	{
		sim_res_type = MEDIGUN_NUM_RESISTS;
		goal_res_type = MEDIGUN_NUM_RESISTS;
		changing_res = false;
		pop = false;

		return;
	}

	auto local{ H::Entities->GetLocal() };

	if (!local || local->deadflag())
	{
		sim_res_type = MEDIGUN_NUM_RESISTS;
		goal_res_type = MEDIGUN_NUM_RESISTS;
		changing_res = false;
		pop = false;

		return;
	}

	auto weapon{ H::Entities->GetWeapon() };

	if (!weapon || weapon->GetWeaponID() != TF_WEAPON_MEDIGUN)
	{
		sim_res_type = MEDIGUN_NUM_RESISTS;
		goal_res_type = MEDIGUN_NUM_RESISTS;
		changing_res = false;
		pop = false;

		return;
	}

	auto medigun{ weapon->As<C_WeaponMedigun>() };

	if (!medigun || medigun->GetChargeType() < 3)
	{
		sim_res_type = MEDIGUN_NUM_RESISTS;
		goal_res_type = MEDIGUN_NUM_RESISTS;
		changing_res = false;
		pop = false;

		return;
	}

	auto cur_res_type{ medigun->GetResistType() };

	if (sim_res_type == MEDIGUN_NUM_RESISTS)
	{
		sim_res_type = cur_res_type;
	}

	if (sim_res_type != goal_res_type && goal_res_type != MEDIGUN_NUM_RESISTS)
	{
		changing_res = true;

		if (!(G::nOldButtons & IN_RELOAD))
		{
			cmd->buttons |= IN_RELOAD;

			sim_res_type++;

			if (sim_res_type > 2)
			{
				sim_res_type = 0;
			}
		}

		return;
	}

	else
	{
		changing_res = false;

		goal_res_type = MEDIGUN_NUM_RESISTS;

		if (cmd->buttons & IN_RELOAD)
		{
			cmd->buttons &= ~IN_RELOAD;
		}

		/*if ((cmd->buttons & IN_RELOAD) && !(G::nOldButtons & IN_RELOAD) && G::bCanPrimaryAttack)
		{
			sim_res_type++;

			if (sim_res_type > 2)
			{
				sim_res_type = 0;
			}

			goal_res_type = sim_res_type;
		}*/
	}

	if (pop)
	{
		pop = false;

		cmd->buttons |= IN_ATTACK2;

		return;
	}

	if (!changing_res && !pop)
	{
		auto heal_target{ medigun->m_hHealingTarget().Get()->As<C_TFPlayer>() };

		medigun_resist_types_t danger_type{ MEDIGUN_NUM_RESISTS };

		if (isPlayerInDanger(heal_target, danger_type) && !playerHasResUber(danger_type, heal_target))
		{
			goal_res_type = danger_type;
			
			if (medigun->m_flChargeLevel() >= 0.25f)
			{
				pop = true;

				if (CFG::AutoVaccinator_Pop == 1 && !heal_target->IsPlayerOnSteamFriendsList())
				{
					pop = false;
				}
			}
		}

		else
		{
			if (isPlayerInDanger(local, danger_type) && !playerHasResUber(danger_type, local))
			{
				goal_res_type = danger_type;
				
				if (medigun->m_flChargeLevel() >= 0.25f)
				{
					pop = true;
				}
			}
		}

		if (danger_type != MEDIGUN_NUM_RESISTS)
		{
			goal_res_type = danger_type;
		}
	}
}

void auto_vaccinator::processPlayerHurt(IGameEvent *event)
{
	if (!CFG::AutoVaccinator_Active)
	{
		return;
	}

	if (changing_res || pop)
	{
		return;
	}

	auto local{ H::Entities->GetLocal() };

	if (!local || local->deadflag())
	{
		return;
	}

	auto weapon{ H::Entities->GetWeapon() };

	if (!weapon || weapon->GetWeaponID() != TF_WEAPON_MEDIGUN)
	{
		return;
	}

	auto medigun{ weapon->As<C_WeaponMedigun>() };

	if (!medigun || medigun->GetChargeType() < 3)
	{
		return;
	}

	auto victim{ GET_ENT_FROM_USER_ID(event->GetInt("userid")) };
	auto attacker{ GET_ENT_FROM_USER_ID(event->GetInt("attacker")) };

	if (!victim || victim == attacker)
	{
		return;
	}

	auto health{ event->GetInt("health") };

	if (health <= 0)
	{
		return;
	}

	auto weapon_id{ event->GetInt("weaponid") };
	
	if (!weapon_id)
	{
		return;
	}

	if (victim != local && victim != medigun->m_hHealingTarget())
	{
		return;
	}

	auto victim_ent{ victim->As<C_TFPlayer>() };

	if (!victim_ent)
	{
		return;
	}

	auto percent_health{ Math::RemapValClamped
	(
		static_cast<float>(health), 0.0f, static_cast<float>(victim_ent->GetMaxHealth()), 0.0f, 1.0f
	) };

	goal_res_type = weaponIDToResType(weapon_id);

	if (medigun->m_flChargeLevel() >= 0.25f)
	{
		if (goal_res_type == MEDIGUN_BULLET_RESIST && !playerHasResUber(MEDIGUN_BULLET_RESIST, victim_ent))
		{
			if (event->GetBool("crit") || event->GetBool("minicrit") || percent_health < 0.85f)
			{
				pop = true;
			}
		}

		if (!pop && goal_res_type == MEDIGUN_FIRE_RESIST && !playerHasResUber(MEDIGUN_FIRE_RESIST, victim_ent))
		{
			if (event->GetBool("crit") || event->GetBool("minicrit") || percent_health < 0.85f)
			{
				pop = true;
			}
		}
	}

	auto heal_target{ medigun->m_hHealingTarget().Get() };

	if (victim == heal_target && CFG::AutoVaccinator_Pop == 1 && !heal_target->As<C_TFPlayer>()->IsPlayerOnSteamFriendsList())
	{
		pop = false;
	}
}

void auto_vaccinator::preventReload(CUserCmd *cmd)
{
	if (!CFG::AutoVaccinator_Active || changing_res)
	{
		return;
	}

	auto weapon{ H::Entities->GetWeapon() };

	if (!weapon || weapon->GetWeaponID() != TF_WEAPON_MEDIGUN)
	{
		return;
	}

	auto medigun{ weapon->As<C_WeaponMedigun>() };

	if (!medigun || medigun->GetChargeType() < 3)
	{
		return;
	}

	if (cmd->buttons & IN_RELOAD)
	{
		cmd->buttons &= ~IN_RELOAD;
	}
}