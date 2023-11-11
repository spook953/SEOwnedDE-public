#include "AimbotMelee.h"

#include "../../CFG.h"
#include "../../AimUtils/AimUtils.h"

bool CAimbotMelee::CanSee(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, Target_t &Target)
{
	if (pLocal->GetShootPos().DistTo(Target.m_vPosition) > 600.0f)
		return false;

	auto Check = [&](const Vec3 &vLocalPos) -> bool
	{
		bool bCanSee = false;

		auto vToSee = [&]() {
			Vec3 vForward = Vec3();
			Math::AngleVectors(Target.m_vAngleTo, &vForward);
			return vLocalPos + (vForward * pWeapon->GetSwingRange());
		}();

		if (Target.m_pLagRecord)
			F::LagRecordMatrixHelper->Set(Target.m_pLagRecord);

		bCanSee = F::AimUtils->TraceEntityMelee(Target.m_pEntity, vLocalPos, vToSee);

		if (CFG::Aimbot_Melee_Aim_Type == 2 || CFG::Aimbot_Melee_Aim_Type == 3)
		{
			auto vToHit = [&]() {
				Vec3 vForward = Vec3();
				Math::AngleVectors(I::EngineClient->GetViewAngles(), &vForward);
				return vLocalPos + (vForward * pWeapon->GetSwingRange());
			}();

			Target.m_bMeleeTraceHit = F::AimUtils->TraceEntityMelee(Target.m_pEntity, vLocalPos, vToHit);
		}

		else Target.m_bMeleeTraceHit = bCanSee;

		if (Target.m_pLagRecord)
			F::LagRecordMatrixHelper->Restore();

		return bCanSee;
	};

	if (Check(pLocal->GetShootPos()))
		return true;

	else
	{
		if (!CFG::Aimbot_Melee_Predict_Swing || pLocal->InCond(TF_COND_SHIELD_CHARGE) || pWeapon->GetWeaponID() == TF_WEAPON_KNIFE)
			return false;

		//TODO: move this to movement simulation at some point
		auto Extrapolate = [](Vec3 &vPos, const Vec3 &vVel, float flTime, bool bGravity) -> void
		{
			if (bGravity)
				vPos += (vVel * flTime) - Vec3(0.0f, 0.0f, SDKUtils::GetGravity()) * 0.5f * flTime * flTime;

			else vPos += (vVel * flTime);
		};

		bool bDoGravity = !(pLocal->m_fFlags() & FL_ONGROUND) && pLocal->GetMoveType() == MOVETYPE_WALK;

		auto amount{ CFG::Aimbot_Melee_Predict_Swing_Amount };

		for (float flTime = 0.0f; flTime < amount; flTime += I::GlobalVars->interval_per_tick)
		{
			Vec3 vLocalPos = pLocal->GetShootPos();

			if (Target.m_pEntity->GetClassId() == ETFClassIds::CTFPlayer)
				Extrapolate(vLocalPos, pLocal->m_vecVelocity() + (Target.m_pEntity->As<C_TFPlayer>()->m_vecVelocity() * -1.0f), flTime, bDoGravity);

			else if (Target.m_pLagRecord)
				Extrapolate(vLocalPos, pLocal->m_vecVelocity() + (Target.m_pLagRecord->m_vVelocity * -1.0f), flTime, bDoGravity);

			else Extrapolate(vLocalPos, pLocal->m_vecVelocity(), flTime, bDoGravity);

			if (Check(vLocalPos))
				return true;
		}
	}

	return false;
}

bool CAimbotMelee::GetTarget(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, Target_t &Out)
{
	Vec3 vLocalPos = pLocal->GetShootPos();
	Vec3 vLocalAngles = I::EngineClient->GetViewAngles();

	m_vecTargets.clear();

	if (CFG::Aimbot_Target_Players)
	{
		auto group{ pWeapon->m_iItemDefinitionIndex() == Soldier_t_TheDisciplinaryAction ? EEntGroup::PLAYERS_ALL : EEntGroup::PLAYERS_ENEMIES };

		if (!CFG::Aimbot_Melee_Whip_Teammates)
		{
			group = EEntGroup::PLAYERS_ENEMIES;
		}

		for (auto pEntity : H::Entities->GetGroup(group))
		{
			if (!pEntity)
				continue;

			auto pPlayer = pEntity->As<C_TFPlayer>();

			if (pPlayer->deadflag() || pPlayer->InCond(TF_COND_HALLOWEEN_GHOST_MODE))
				continue;

			if (pPlayer->m_iTeamNum() != pLocal->m_iTeamNum())
			{
				if (CFG::Aimbot_Ignore_Friends && pPlayer->IsPlayerOnSteamFriendsList())
					continue;

				if (CFG::Aimbot_Ignore_Invisible && pPlayer->IsInvisible())
					continue;

				if (pWeapon->m_iItemDefinitionIndex() != Heavy_t_TheHolidayPunch && CFG::Aimbot_Ignore_Invulnerable && pPlayer->IsInvulnerable())
					continue;

				if (CFG::Aimbot_Ignore_Taunting && pPlayer->InCond(TF_COND_TAUNTING))
					continue;
			}

			if (pPlayer->m_iTeamNum() != pLocal->m_iTeamNum() && CFG::Aimbot_Melee_Target_LagRecords)
			{
				int nRecords = 0;

				if (!F::LagRecords->HasRecords(pPlayer, &nRecords))
					continue;

				for (int n = 1; n < nRecords; n++)
				{
					auto pRecord = F::LagRecords->GetRecord(pPlayer, n, true);

					if (!pRecord || !F::LagRecords->DiffersFromCurrent(pRecord))
						continue;

					Vec3 vPos = SDKUtils::GetHitboxPosFromMatrix(pPlayer, HITBOX_BODY, const_cast<matrix3x4_t *>(pRecord->m_BoneMatrix));
					Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
					float flFOVTo = CFG::Aimbot_Melee_Sort == 0 ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
					float flDistTo = vLocalPos.DistTo(vPos);

					if (CFG::Aimbot_Melee_Sort == 0 && flFOVTo > CFG::Aimbot_Melee_FOV)
						continue;

					m_vecTargets.emplace_back(Target_t{ pPlayer, vPos, vAngleTo, flFOVTo, flDistTo, pRecord->m_flSimulationTime, pRecord });
				}
			}

			Vec3 vPos = pPlayer->GetHitboxPos(HITBOX_BODY);
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			float flFOVTo = CFG::Aimbot_Melee_Sort == 0 ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			float flDistTo = vLocalPos.DistTo(vPos);

			if (CFG::Aimbot_Melee_Sort == 0 && flFOVTo > CFG::Aimbot_Melee_FOV)
				continue;

			m_vecTargets.emplace_back(Target_t{ pPlayer, vPos, vAngleTo, flFOVTo, flDistTo, pPlayer->m_flSimulationTime() });
		}
	}

	if (CFG::Aimbot_Target_Buildings)
	{
		for (auto pEntity : H::Entities->GetGroup(EEntGroup::BUILDINGS_ENEMIES))
		{
			if (!pEntity)
				continue;

			auto pBuilding = pEntity->As<C_BaseObject>();

			if (pBuilding->m_bPlacing())
				continue;

			Vec3 vPos = pBuilding->GetCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			float flFOVTo = CFG::Aimbot_Melee_Sort == 0 ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			float flDistTo = vLocalPos.DistTo(vPos);

			if (CFG::Aimbot_Melee_Sort == 0 && flFOVTo > CFG::Aimbot_Melee_FOV)
				continue;

			m_vecTargets.emplace_back(Target_t{ pBuilding, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	if (m_vecTargets.empty())
		return false;

	std::sort(m_vecTargets.begin(), m_vecTargets.end(), [&](const Target_t &a, const Target_t &b) -> bool
	{
		switch (CFG::Aimbot_Melee_Sort)
		{
			case 0: return a.m_flFOVTo < b.m_flFOVTo;
			case 1: return a.m_flDistanceTo < b.m_flDistanceTo;
			default: return false;
		}
	});

	const int it_end = std::min(4, static_cast<int>(m_vecTargets.size()));

	for (int n = 0; n < it_end; n++)
	{
		auto &Target = m_vecTargets[n];

		if (!CanSee(pLocal, pWeapon, Target))
			continue;

		Out = Target;
		return true;
	}

	return false;
}

bool CAimbotMelee::ShouldAim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Target_t &Target)
{
	if (CFG::Aimbot_Melee_Aim_Type == 1 && !IsFiring(pCmd, pWeapon))
		return false;

	return true;
}

void CAimbotMelee::Aim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Vec3 &vAngles)
{
	Vec3 vAngleTo = vAngles - pLocal->m_vecPunchAngle();
	Math::ClampAngles(vAngleTo);

	switch (CFG::Aimbot_Melee_Aim_Type)
	{
		case 0:
		{
			pCmd->viewangles = vAngleTo;
			break;
		}

		case 1:
		{
			if (IsFiring(pCmd, pWeapon))
			{
				F::AimUtils->FixMovement(pCmd, vAngleTo);
				pCmd->viewangles = vAngleTo;

				if (Shifting::bShifting && Shifting::bShiftingWarp)
					G::bSilentAngles = true;

				else G::bPSilentAngles = true;
			}

			break;
		}

		case 2:
		{
			Vec3 vDelta = vAngleTo - pCmd->viewangles;
			Math::ClampAngles(vDelta);

			if (vDelta.Length() > 0.0f && CFG::Aimbot_Melee_Smoothing)
				pCmd->viewangles += vDelta / CFG::Aimbot_Melee_Smoothing;

			break;
		}

		default: break;
	}
}

bool CAimbotMelee::ShouldFire(Target_t &Target)
{
	if (!CFG::Aimbot_AutoShoot)
		return false;

	return Target.m_bMeleeTraceHit;
}

void CAimbotMelee::HandleFire(CUserCmd *pCmd, C_TFWeaponBase *pWeapon, const Target_t &Target)
{
	pCmd->buttons |= IN_ATTACK;
}

bool CAimbotMelee::IsFiring(CUserCmd *pCmd, C_TFWeaponBase *pWeapon)
{
	if (Shifting::bShifting && Shifting::bShiftingWarp)
		return true;

	if (pWeapon->GetWeaponID() == TF_WEAPON_KNIFE)
		return (pCmd->buttons & IN_ATTACK) && G::bCanPrimaryAttack;
	
	return fabsf(pWeapon->m_flSmackTime() - I::GlobalVars->curtime) < I::GlobalVars->interval_per_tick * 2.0f;
}

void CAimbotMelee::Run(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon)
{
	if (!CFG::Aimbot_Melee_Active)
		return;

	if (CFG::Aimbot_Melee_Sort == 0)
		G::flAimbotFOV = CFG::Aimbot_Melee_FOV;

	if (Shifting::bShifting && !Shifting::bShiftingWarp)
		return;

	bool is_firing{ IsFiring(pCmd, pWeapon) };

	Target_t Target = {};

	if (GetTarget(pLocal, pWeapon, Target) && Target.m_pEntity)
	{
		auto key_down{ H::Input->IsDown(CFG::Aimbot_Key) || CFG::Aimbot_Melee_Always_Active };

		if (key_down || is_firing)
		{
			G::nTargetIndex = Target.m_pEntity->entindex();

			if (key_down)
			{
				if (ShouldFire(Target))
					HandleFire(pCmd, pWeapon, Target);
			}

			bool bIsFiring = IsFiring(pCmd, pWeapon);

			G::bFiring = bIsFiring;

			if (ShouldAim(pCmd, pLocal, pWeapon, Target) || bIsFiring)
			{
				if (key_down)
				{
					Aim(pCmd, pLocal, pWeapon, Target.m_vAngleTo);
				}

				if (CFG::Misc_Accuracy_Improvements)
				{
					if (bIsFiring && Target.m_pEntity->GetClassId() == ETFClassIds::CTFPlayer)
						pCmd->tick_count = TIME_TO_TICKS(Target.m_flSimulationTime + SDKUtils::GetLerp());
				}

				else
				{
					if (bIsFiring && Target.m_pLagRecord)
					{
						pCmd->tick_count = TIME_TO_TICKS(Target.m_flSimulationTime + GetClientInterpAmount());
					}
				}
			}

			if (CFG::Aimbot_Melee_Walk_To_Target && (pLocal->m_fFlags() & FL_ONGROUND))
			{
				auto WalkTo = [&](const Vec3 &vFrom, const Vec3 &vTo) -> void
				{
					Vec3 vDelta = vTo - vFrom;

					if (vDelta.Length() == 0.0f)
						return;

					Vec3 vDeltaMove = { vDelta.x, vDelta.y, 0.0f }, vDeltaDir = {};
					Math::VectorAngles(vDeltaMove, vDeltaDir);
					float flYaw = DEG2RAD(vDeltaDir.y - pCmd->viewangles.y);
					pCmd->forwardmove = cosf(flYaw) * 450.0f;
					pCmd->sidemove = -sinf(flYaw) * 450.0f;
				};

				WalkTo(pLocal->m_vecOrigin(), Target.m_vPosition);
			}
		}
	}
}