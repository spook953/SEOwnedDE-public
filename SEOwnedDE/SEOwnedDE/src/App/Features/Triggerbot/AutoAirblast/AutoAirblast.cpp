#include "AutoAirblast.h"

#include "../../CFG.h"

#include "../../AimUtils/AimUtils.h"

struct TargetProjectile
{
	C_BaseProjectile *ptr{};
	Vec3 pos{};
};

bool findTargetProjectile(C_TFPlayer *local, TargetProjectile &out)
{
	for (auto ent : H::Entities->GetGroup(EEntGroup::PROJECTILES_ENEMIES))
	{
		if (!ent)
		{
			continue;
		}

		auto proj{ ent->As<C_BaseProjectile>() };

		if (!proj)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_Rocket && proj->GetClassId() == ETFClassIds::CTFProjectile_Rocket)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_SentryRocket && proj->GetClassId() == ETFClassIds::CTFProjectile_SentryRocket)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_Jar && proj->GetClassId() == ETFClassIds::CTFProjectile_Jar)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_JarGas && proj->GetClassId() == ETFClassIds::CTFProjectile_JarGas)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_JarMilk && proj->GetClassId() == ETFClassIds::CTFProjectile_JarMilk)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_Arrow && proj->GetClassId() == ETFClassIds::CTFProjectile_Arrow)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_Flare && proj->GetClassId() == ETFClassIds::CTFProjectile_Flare)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_Cleaver && proj->GetClassId() == ETFClassIds::CTFProjectile_Cleaver)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_HealingBolt && proj->GetClassId() == ETFClassIds::CTFProjectile_HealingBolt)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_PipebombProjectile && proj->GetClassId() == ETFClassIds::CTFGrenadePipebombProjectile)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_BallOfFire && proj->GetClassId() == ETFClassIds::CTFProjectile_BallOfFire)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_EnergyRing && proj->GetClassId() == ETFClassIds::CTFProjectile_EnergyRing)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Ignore_EnergyBall && proj->GetClassId() == ETFClassIds::CTFProjectile_EnergyBall)
		{
			continue;
		}

		Vec3 vel{};

		proj->EstimateAbsVelocity(vel);

		if (proj->GetClassId() == ETFClassIds::CTFGrenadePipebombProjectile
			&& (proj->As<C_TFGrenadePipebombProjectile>()->m_bTouched()
				|| proj->As<C_TFGrenadePipebombProjectile>()->m_iType() == TF_PROJECTILE_PIPEBOMB_PRACTICE))
		{
			continue;
		}

		if (proj->GetClassId() == ETFClassIds::CTFProjectile_Arrow && fabsf(vel.Length()) <= 10.0f)
		{
			continue;
		}

		auto pos{ proj->m_vecOrigin() + (vel * SDKUtils::GetLatency()) };

		if (pos.DistTo(local->GetShootPos()) > 160.0f)
		{
			continue;
		}

		if (CFG::Triggerbot_AutoAirblast_Mode == 0
			&& Math::CalcFov(I::EngineClient->GetViewAngles(), Math::CalcAngle(local->GetShootPos(), pos)) > 60.0f)
		{
			continue;
		}

		CTraceFilterWorldCustom filter{};
		trace_t trace{};

		F::AimUtils->Trace(local->GetShootPos(), pos, MASK_SOLID, &filter, &trace);

		if (trace.fraction < 1.0f || trace.allsolid || trace.startsolid)
		{
			continue;
		}

		out.ptr = proj;
		out.pos = pos;

		return true;
	}

	return false;
}

void CAutoAirblast::Run(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, CUserCmd *pCmd)
{
	if (!CFG::Triggerbot_AutoAirblast_Active)
	{
		return;
	}

	if (!G::bCanSecondaryAttack || (pWeapon->GetWeaponID() != TF_WEAPON_FLAMETHROWER && pWeapon->GetWeaponID() != TF_WEAPON_FLAME_BALL)
		|| pWeapon->m_iItemDefinitionIndex() == Pyro_m_ThePhlogistinator)
	{
		return;
	}

	TargetProjectile target_projectile{};

	if (!findTargetProjectile(pLocal, target_projectile))
	{
		return;
	}

	pCmd->buttons |= IN_ATTACK2;

	if (CFG::Triggerbot_AutoAirblast_Mode == 1)
	{
		if (CFG::Triggerbot_AutoAirblast_Aim_Assist)
		{
			auto getOwner = [](C_BaseProjectile *proj) -> C_BaseEntity *
			{
				auto owner{ proj->m_hOwnerEntity().Get() };

				if (owner)
				{
					return owner;
				}

				else
				{
					if (proj->GetClassId() == ETFClassIds::CTFGrenadePipebombProjectile)
					{
						auto launcher{ proj->As<C_TFGrenadePipebombProjectile>()->m_hLauncher().Get() };

						if (launcher)
						{
							return launcher->m_hOwnerEntity().Get();
						}
					}
				}

				return nullptr;
			};

			auto owner{ getOwner(target_projectile.ptr) };

			if (owner && owner->GetClassId() == ETFClassIds::CTFPlayer)
			{
				CTraceFilterWorldCustom filter{};
				trace_t trace{};

				F::AimUtils->Trace(pLocal->GetShootPos(), owner->GetCenter(), MASK_SOLID, &filter, &trace);

				if (trace.fraction > 0.99f && !trace.allsolid && !trace.startsolid)
				{
					pCmd->viewangles = Math::CalcAngle(pLocal->GetShootPos(), owner->GetCenter());
				}

				else
				{
					pCmd->viewangles = Math::CalcAngle(pLocal->GetShootPos(), target_projectile.pos);
				}
			}
		}

		else
		{
			pCmd->viewangles = Math::CalcAngle(pLocal->GetShootPos(), target_projectile.pos);
		}

		if (CFG::Triggerbot_AutoAirblast_Aim_Mode == 1)
		{
			G::bSilentAngles = true;
		}
	}
}