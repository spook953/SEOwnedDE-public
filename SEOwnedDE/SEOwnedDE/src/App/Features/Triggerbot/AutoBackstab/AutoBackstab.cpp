#include "AutoBackstab.h"

#include "../../CFG.h"

#include "../../AimUtils/AimUtils.h"
#include "../../LagRecords/LagRecords.h"

bool IsBehindAndFacingTarget(const Vec3 &owner_center, const Vec3 &owner_viewangles, const Vec3 &target_center, const Vec3 &target_eyeangles)
{
	Vec3 to_target{};
	to_target = target_center - owner_center;
	to_target.z = 0.0f;
	to_target.NormalizeInPlace();

	Vec3 owner_forward{};
	Math::AngleVectors(owner_viewangles, &owner_forward, nullptr, nullptr);
	owner_forward.z = 0.0f;
	owner_forward.NormalizeInPlace();

	Vec3 target_forward{};
	Math::AngleVectors(target_eyeangles, &target_forward, nullptr, nullptr);
	target_forward.z = 0.0f;
	target_forward.NormalizeInPlace();

	return to_target.Dot(target_forward) > (0.0f + 0.03125f)
		&& to_target.Dot(owner_forward) > (0.5f + 0.03125f)
		&& target_forward.Dot(owner_forward) > (-0.3f + 0.03125f);
}

bool CanKnifeOneShot(C_TFPlayer *target, bool crit, bool mini_crit)
{
	if (!target || target->IsInvulnerable())
	{
		return false;
	}

	auto weapon{ target->m_hActiveWeapon().Get() };

	if (!weapon)
	{
		return false;
	}

	auto dmg_mult{ 1 };

	if (mini_crit || target->IsMarked())
	{
		dmg_mult = 2;
	}

	if (crit)
	{
		dmg_mult = 3;
	}

	if (weapon->As<C_TFWeaponBase>()->m_iItemDefinitionIndex() == Heavy_t_FistsofSteel)
	{
		return target->m_iHealth() <= 80 * dmg_mult;
	}

	return target->m_iHealth() <= 40 * dmg_mult;
}

void CAutoBackstab::Run(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, CUserCmd *pCmd)
{
	if (!CFG::Triggerbot_AutoBackstab_Active)
	{
		return;
	}

	if (!G::bCanPrimaryAttack || pLocal->m_bFeignDeathReady() || pLocal->m_flInvisibility() > 0.0f || pWeapon->GetWeaponID() != TF_WEAPON_KNIFE)
	{
		return;
	}

	for (auto ent : H::Entities->GetGroup(EEntGroup::PLAYERS_ENEMIES))
	{
		if (!ent)
		{
			continue;
		}

		auto player{ ent->As<C_TFPlayer>() };

		if (!player || player->deadflag() || player->InCond(TF_COND_HALLOWEEN_GHOST_MODE))
		{
			continue;
		}

		if (CFG::Triggerbot_AutoBackstab_Ignore_Friends && player->IsPlayerOnSteamFriendsList())
		{
			continue;
		}

		if (CFG::Triggerbot_AutoBackstab_Ignore_Invisible && player->IsInvisible())
		{
			continue;
		}

		if (CFG::Triggerbot_AutoBackstab_Ignore_Invulnerable && player->IsInvulnerable())
		{
			continue;
		}

		auto can_knife{ CanKnifeOneShot(player, pLocal->IsCritBoosted(), pLocal->IsMiniCritBoosted()) };

		if (!CFG::Triggerbot_AutoBackstab_Knife_If_Lethal)
		{
			can_knife = false;
		}

		auto angle_to{ I::EngineClient->GetViewAngles() };

		if (CFG::Triggerbot_AutoBacktab_Mode == 1)
		{
			angle_to = Math::CalcAngle(pLocal->GetShootPos(), player->GetCenter());
		}

		if (can_knife || IsBehindAndFacingTarget(pLocal->GetCenter(), angle_to, player->GetCenter(), player->GetEyeAngles()))
		{
			Vec3 forward{};
			Math::AngleVectors(angle_to, &forward);

			auto to{ pLocal->GetShootPos() + (forward * 47.0f) };

			if (F::AimUtils->TraceEntityMelee(player, pLocal->GetShootPos(), to))
			{
				if (CFG::Triggerbot_AutoBacktab_Mode == 1)
				{
					pCmd->viewangles = angle_to;

					if (CFG::Triggerbot_AutoBacktab_Aim_Mode == 1)
					{
						G::bPSilentAngles = true;
					}
				}

				pCmd->buttons |= IN_ATTACK;

				if (CFG::Misc_Accuracy_Improvements)
				{
					pCmd->tick_count = TIME_TO_TICKS(player->m_flSimulationTime() + SDKUtils::GetLerp());
				}

				return;
			}
		}

		int num_records{};

		if (!F::LagRecords->HasRecords(player, &num_records))
		{
			continue;
		}

		for (int n{ 1 }; n < num_records; n++)
		{
			auto record{ F::LagRecords->GetRecord(player, n, true) };

			if (!record)
			{
				continue;
			}

			if (CFG::Triggerbot_AutoBacktab_Mode == 1)
			{
				angle_to = Math::CalcAngle(pLocal->GetShootPos(), record->m_vCenter);
			}

			if (can_knife || IsBehindAndFacingTarget(pLocal->GetCenter(), angle_to, record->m_vCenter, player->GetEyeAngles()))
			{
				F::LagRecordMatrixHelper->Set(record);

				Vec3 forward{};
				Math::AngleVectors(angle_to, &forward);

				auto to{ pLocal->GetShootPos() + (forward * 47.0f) };

				if (!F::AimUtils->TraceEntityMelee(player, pLocal->GetShootPos(), to))
				{
					F::LagRecordMatrixHelper->Restore();

					continue;
				}

				F::LagRecordMatrixHelper->Restore();

				if (CFG::Triggerbot_AutoBacktab_Mode == 1)
				{
					pCmd->viewangles = angle_to;

					if (CFG::Triggerbot_AutoBacktab_Aim_Mode == 1)
					{
						G::bPSilentAngles = true;
					}
				}

				pCmd->buttons |= IN_ATTACK;

				if (CFG::Misc_Accuracy_Improvements)
				{
					pCmd->tick_count = TIME_TO_TICKS(record->m_flSimulationTime + SDKUtils::GetLerp());
				}

				else
				{
					pCmd->tick_count = TIME_TO_TICKS(record->m_flSimulationTime + GetClientInterpAmount());
				}

				return;
			}
		}
	}
}