#include "Aimbot.h"

#include "AimbotHitscan/AimbotHitscan.h"
#include "AimbotProjectile/AimbotProjectile.h"
#include "AimbotMelee/AimbotMelee.h"

#include "../AimUtils/AimUtils.h"

#include "../CFG.h"

void CAimbot::RunMain(CUserCmd *pCmd)
{
	G::nTargetIndex = -1;
	G::flAimbotFOV = 0.0f;
	G::nTargetIndexEarly = -1;

	if (!CFG::Aimbot_Active || I::EngineVGui->IsGameUIVisible() || I::MatSystemSurface->IsCursorVisible() || SDKUtils::BInEndOfMatch())
		return;

	if (Shifting::bRecharging)
		return;

	auto pLocal = H::Entities->GetLocal();
	auto pWeapon = H::Entities->GetWeapon();

	if (!pLocal || !pWeapon
		|| pLocal->deadflag()
		|| pLocal->InCond(TF_COND_TAUNTING) || pLocal->InCond(TF_COND_PHASE)
		|| pLocal->InCond(TF_COND_HALLOWEEN_GHOST_MODE)
		|| pLocal->InCond(TF_COND_HALLOWEEN_BOMB_HEAD)
		|| pLocal->InCond(TF_COND_HALLOWEEN_KART)
		|| pLocal->m_bFeignDeathReady() || pLocal->m_flInvisibility() > 0.0f
		|| pWeapon->m_iItemDefinitionIndex() == Soldier_m_RocketJumper || pWeapon->m_iItemDefinitionIndex() == Demoman_s_StickyJumper)
		return;

	switch (F::AimUtils->GetWeaponType(pWeapon))
	{
		case EWeaponType::HITSCAN: {
			F::AimbotHitscan->Run(pCmd, pLocal, pWeapon);
			break;
		}

		case EWeaponType::PROJECTILE: {
			F::AimbotProjectile->Run(pCmd, pLocal, pWeapon);
			break;
		}

		case EWeaponType::MELEE: {
			F::AimbotMelee->Run(pCmd, pLocal, pWeapon);
			break;
		}

		default: break;
	}
}

void CAimbot::Run(CUserCmd *pCmd)
{
	RunMain(pCmd);

	//same-ish code below to see if we are firing manually

	auto pLocal = H::Entities->GetLocal();
	auto pWeapon = H::Entities->GetWeapon();

	if (!pLocal || !pWeapon
		|| pLocal->deadflag()
		|| pLocal->InCond(TF_COND_TAUNTING) || pLocal->InCond(TF_COND_PHASE)
		|| pLocal->m_bFeignDeathReady() || pLocal->m_flInvisibility() > 0.0f)
		return;

	auto nWeaponType = F::AimUtils->GetWeaponType(pWeapon);

	if (!G::bFiring)
	{
		switch (nWeaponType)
		{
			case EWeaponType::HITSCAN: {
				G::bFiring = F::AimbotHitscan->IsFiring(pCmd, pWeapon);
				break;
			}

			case EWeaponType::PROJECTILE: {
				G::bFiring = F::AimbotProjectile->IsFiring(pCmd, pLocal, pWeapon);
				break;
			}

			case EWeaponType::MELEE: {
				G::bFiring = F::AimbotMelee->IsFiring(pCmd, pWeapon);
				break;
			}

			default: break;
		}
	}

	if (G::bFiring && nWeaponType == EWeaponType::PROJECTILE && CFG::Aimbot_Projectile_NoSpread)
	{
		SDKUtils::SharedRandomInt("SelectWeightedSequence", 0, 0, pCmd->random_seed);

		for (int i = 0; i < 6; ++i)
			I::UniformRandomStream->RandomFloat();

		switch (pWeapon->GetWeaponID())
		{
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_CANNON:
			{
				pCmd->command_number = SDKUtils::FindCmdNumWithSeed(pCmd->command_number, 39);
				break;
			}

			case TF_WEAPON_SYRINGEGUN_MEDIC:
			{
				pCmd->viewangles.x -= I::UniformRandomStream->RandomFloat(-1.5f, 1.5f);
				pCmd->viewangles.y -= I::UniformRandomStream->RandomFloat(-1.5f, 1.5f);
				Math::ClampAngles(pCmd->viewangles);
				G::bPSilentAngles = true;
				break;
			}

			case TF_WEAPON_COMPOUND_BOW:
			{
				Vec3 vSpread = {}, vSrc = {};
				pWeapon->GetProjectileFireSetup(pLocal, { 0.0f, 0.0f, 0.0f }, &vSrc, &vSpread, false, 2000.0f);
				pCmd->viewangles -= (vSpread - I::EngineClient->GetViewAngles());
				Math::ClampAngles(pCmd->viewangles);
				G::bPSilentAngles = true;
				break;
			}

			default:
			{
				if (pWeapon->m_iItemDefinitionIndex() == Soldier_m_TheBeggarsBazooka)
				{
					Vec3 vSpread = {};
					pWeapon->GetSpreadAngles(vSpread);
					pCmd->viewangles -= (vSpread - I::EngineClient->GetViewAngles());
					Math::ClampAngles(pCmd->viewangles);
					G::bPSilentAngles = true;
				}

				break;
			}
		}
	}
}