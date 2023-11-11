#include "AimbotHitscan.h"

#include "../../AimUtils/AimUtils.h"
#include "../../CFG.h"

int CAimbotHitscan::GetAimHitbox(C_TFWeaponBase *pWeapon)
{
	switch (CFG::Aimbot_Hitscan_Hitbox)
	{
		case 0: return HITBOX_HEAD;
		case 1: return HITBOX_PELVIS;
		case 2: 
		{
			if (pWeapon->GetWeaponID() == TF_WEAPON_SNIPERRIFLE_CLASSIC)
				return (pWeapon->As<C_TFSniperRifle>()->m_flChargedDamage() >= 150.0f) ? HITBOX_HEAD : HITBOX_PELVIS;

			return F::AimUtils->IsWeaponCapableOfHeadshot(pWeapon) ? HITBOX_HEAD : HITBOX_PELVIS;
		}
		default: return HITBOX_PELVIS;
	}
	
	return 0;
}

bool CAimbotHitscan::ScanHead(C_TFPlayer *pLocal, Target_t &Target)
{
	if (!CFG::Aimbot_Hitscan_Scan_Head)
		return false;

	auto pPlayer = Target.m_pEntity->As<C_TFPlayer>();

	if (!pPlayer)
		return false;

	auto pModel = pPlayer->GetModel();

	if (!pModel)
		return false;

	auto pHDR = I::ModelInfoClient->GetStudiomodel(pModel);

	if (!pHDR)
		return false;

	auto pSet = pHDR->pHitboxSet(pPlayer->m_nHitboxSet());

	if (!pSet)
		return false;

	auto pBox = pSet->pHitbox(HITBOX_HEAD);

	if (!pBox)
		return false;

	matrix3x4_t BoneMatrix[128] = {};

	if (!pPlayer->SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime))
		return false;

	Vec3 vMins = pBox->bbmin;
	Vec3 vMaxs = pBox->bbmax;

	std::array<Vec3, 1> vPoints = {
		Vec3((vMins.x + vMaxs.x) * 0.5f, vMins.y * 0.7f, (vMins.z + vMaxs.z) * 0.5f)
	};

	Vec3 vLocalPos = pLocal->GetShootPos();

	for (const auto &vPoint : vPoints)
	{
		Vec3 vTransformed = {};
		Math::VectorTransform(vPoint, BoneMatrix[pBox->bone], vTransformed);

		int nHitHitbox = -1;

		if (!F::AimUtils->TraceEntityBullet(pPlayer, vLocalPos, vTransformed, &nHitHitbox))
			continue;

		if (nHitHitbox != HITBOX_HEAD)
			continue;

		Target.m_vPosition = vTransformed;
		Target.m_vAngleTo = Math::CalcAngle(vLocalPos, vTransformed);
		Target.m_bWasMultiPointed = true;

		return true;
	}

	return false;
}

bool CAimbotHitscan::ScanBody(C_TFPlayer *pLocal, Target_t &Target)
{
	bool bScanningBody = CFG::Aimbot_Hitscan_Scan_Body;
	bool bScaningArms = CFG::Aimbot_Hitscan_Scan_Arms;
	bool bScanningLegs = CFG::Aimbot_Hitscan_Scan_Legs;

	if (!bScanningBody && !bScaningArms && !bScanningLegs)
		return false;

	auto pPlayer = Target.m_pEntity->As<C_TFPlayer>();

	if (!pPlayer)
		return false;

	Vec3 vLocalPos = pLocal->GetShootPos();

	for (int n = 1; n < pPlayer->GetNumOfHitboxes(); n++)
	{
		if (n == Target.m_nAimedHitbox)
			continue;

		int nHitboxGroup = pPlayer->GetHitboxGroup(n);

		if (!bScanningBody && (nHitboxGroup == HITGROUP_CHEST || nHitboxGroup == HITGROUP_STOMACH))
			continue;

		if (!bScaningArms && (nHitboxGroup == HITGROUP_LEFTARM || nHitboxGroup == HITGROUP_RIGHTARM))
			continue;

		if (!bScanningLegs && (nHitboxGroup == HITGROUP_LEFTLEG || nHitboxGroup == HITGROUP_RIGHTLEG))
			continue;

		Vec3 vHitbox = pPlayer->GetHitboxPos(n);

		if (!F::AimUtils->TraceEntityBullet(pPlayer, vLocalPos, vHitbox))
			continue;

		Target.m_vPosition = vHitbox;
		Target.m_vAngleTo = Math::CalcAngle(vLocalPos, vHitbox);

		return true;
	}

	return false;
}

bool CAimbotHitscan::ScanBuilding(C_TFPlayer *pLocal, Target_t &Target)
{
	if (!CFG::Aimbot_Hitscan_Scan_Buildings)
		return false;

	auto pObject = Target.m_pEntity->As<C_BaseObject>();

	if (!pObject)
		return false;

	Vec3 vLocalPos = pLocal->GetShootPos();

	if (pObject->GetClassId() == ETFClassIds::CObjectSentrygun)
	{
		for (int n = 0; n < pObject->GetNumOfHitboxes(); n++)
		{
			Vec3 vHitbox = pObject->GetHitboxPos(n);

			if (!F::AimUtils->TraceEntityBullet(pObject, vLocalPos, vHitbox))
				continue;

			Target.m_vPosition = vHitbox;
			Target.m_vAngleTo = Math::CalcAngle(vLocalPos, vHitbox);

			return true;
		}
	}

	else
	{
		Vec3 vMins = pObject->m_vecMins();
		Vec3 vMaxs = pObject->m_vecMaxs();

		std::array<Vec3, 6> vPoints = {
			Vec3(vMins.x * 0.9f, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
			Vec3(vMaxs.x * 0.9f, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
			Vec3(((vMins.x + vMaxs.x) * 0.5f), vMins.y * 0.9f, ((vMins.z + vMaxs.z) * 0.5f)),
			Vec3(((vMins.x + vMaxs.x) * 0.5f), vMaxs.y * 0.9f, ((vMins.z + vMaxs.z) * 0.5f)),
			Vec3(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMins.z * 0.9f),
			Vec3(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMaxs.z * 0.9f)
		};

		const matrix3x4_t &Transform = pObject->RenderableToWorldTransform();

		for (const auto &Point : vPoints)
		{
			Vec3 vTransformed = {};
			Math::VectorTransform(Point, Transform, vTransformed);

			if (!F::AimUtils->TraceEntityBullet(pObject, vLocalPos, vTransformed))
				continue;

			Target.m_vPosition = vTransformed;
			Target.m_vAngleTo = Math::CalcAngle(vLocalPos, vTransformed);

			return true;
		}
	}

	return false;
}

bool CAimbotHitscan::GetTarget(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, Target_t &Out)
{
	Vec3 vLocalPos = pLocal->GetShootPos();
	Vec3 vLocalAngles = I::EngineClient->GetViewAngles();

	m_vecTargets.clear();

	if (CFG::Aimbot_Target_Players)
	{
		int nAimHitbox = GetAimHitbox(pWeapon);
		
		for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ENEMIES))
		{
			if (!pEntity)
				continue;

			auto pPlayer = pEntity->As<C_TFPlayer>();

			if (pPlayer->deadflag() || pPlayer->InCond(TF_COND_HALLOWEEN_GHOST_MODE))
				continue;

			if (CFG::Aimbot_Ignore_Friends && pPlayer->IsPlayerOnSteamFriendsList())
				continue;

			if (CFG::Aimbot_Ignore_Invisible && pPlayer->IsInvisible())
				continue;

			if (CFG::Aimbot_Ignore_Invulnerable && pPlayer->IsInvulnerable())
				continue;

			if (CFG::Aimbot_Ignore_Taunting && pPlayer->InCond(TF_COND_TAUNTING))
				continue;

			if (CFG::Aimbot_Hitscan_Target_LagRecords)
			{
				int nRecords = 0;

				if (!F::LagRecords->HasRecords(pPlayer, &nRecords))
					continue;

				for (int n = 1; n < nRecords; n++)
				{
					auto pRecord = F::LagRecords->GetRecord(pPlayer, n, true);

					if (!pRecord || !F::LagRecords->DiffersFromCurrent(pRecord))
						continue;

					Vec3 vPos = SDKUtils::GetHitboxPosFromMatrix(pPlayer, nAimHitbox, const_cast<matrix3x4_t *>(pRecord->m_BoneMatrix));
					Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
					float flFOVTo = CFG::Aimbot_Hitscan_Sort == 0 ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
					float flDistTo = vLocalPos.DistTo(vPos);

					if (CFG::Aimbot_Hitscan_Sort == 0 && flFOVTo > CFG::Aimbot_Hitscan_FOV)
						continue;

					m_vecTargets.emplace_back(Target_t{ pPlayer, vPos, vAngleTo, flFOVTo, flDistTo, nAimHitbox, pRecord->m_flSimulationTime, pRecord });
				}
			}

			/*if (CFG::Aimbot_Hitscan_Aim_Type != 2)
			{
				if (TIME_TO_TICKS(pPlayer->m_flSimulationTime() - pPlayer->m_flOldSimulationTime()) < 1)
					continue;
			}*/

			Vec3 vPos = pPlayer->GetHitboxPos(nAimHitbox);
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			float flFOVTo = CFG::Aimbot_Hitscan_Sort == 0 ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			float flDistTo = vLocalPos.DistTo(vPos);

			if (CFG::Aimbot_Hitscan_Sort == 0 && flFOVTo > CFG::Aimbot_Hitscan_FOV)
				continue;

			m_vecTargets.emplace_back(Target_t{ pPlayer, vPos, vAngleTo, flFOVTo, flDistTo, nAimHitbox, pPlayer->m_flSimulationTime()});
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
			float flFOVTo = CFG::Aimbot_Hitscan_Sort == 0 ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			float flDistTo = vLocalPos.DistTo(vPos);

			if (CFG::Aimbot_Hitscan_Sort == 0 && flFOVTo > CFG::Aimbot_Hitscan_FOV)
				continue;

			m_vecTargets.emplace_back(Target_t{ pBuilding, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	if (CFG::Aimbot_Hitscan_Target_Stickies)
	{
		for (auto ent : H::Entities->GetGroup(EEntGroup::PROJECTILES_ENEMIES))
		{
			if (!ent || ent->GetClassId() != ETFClassIds::CTFGrenadePipebombProjectile)
			{
				continue;
			}

			auto pipe{ ent->As<C_TFGrenadePipebombProjectile>() };

			if (!pipe || !pipe->m_bTouched() || !pipe->HasStickyEffects() || pipe->m_iType() == TF_GL_MODE_REMOTE_DETONATE_PRACTICE)
			{
				continue;
			}

			Vec3 vPos = pipe->GetCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			float flFOVTo = CFG::Aimbot_Hitscan_Sort == 0 ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			float flDistTo = vLocalPos.DistTo(vPos);

			if (CFG::Aimbot_Hitscan_Sort == 0 && flFOVTo > CFG::Aimbot_Hitscan_FOV)
				continue;

			m_vecTargets.emplace_back(Target_t{ pipe, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	if (m_vecTargets.empty())
		return false;

	std::sort(m_vecTargets.begin(), m_vecTargets.end(), [&](const Target_t &a, const Target_t &b) -> bool
	{
		switch (CFG::Aimbot_Hitscan_Sort)
		{
			case 0: return a.m_flFOVTo < b.m_flFOVTo;
			case 1: return a.m_flDistanceTo < b.m_flDistanceTo;
			default: return false;
		}
	});

	for (auto &Target : m_vecTargets)
	{
		switch (Target.m_pEntity->GetClassId())
		{
			case ETFClassIds::CTFPlayer:
			{
				if (!Target.m_pLagRecord)
				{
					int nHitHitbox = -1;

					if (!F::AimUtils->TraceEntityBullet(Target.m_pEntity, vLocalPos, Target.m_vPosition, &nHitHitbox))
					{
						if (Target.m_nAimedHitbox == HITBOX_HEAD)
						{
							if (!ScanHead(pLocal, Target))
								continue;
						}

						else if (Target.m_nAimedHitbox == HITBOX_PELVIS)
						{
							if (!ScanBody(pLocal, Target))
								continue;
						}

						else continue;
					}

					else
					{
						if (nHitHitbox != Target.m_nAimedHitbox && Target.m_nAimedHitbox == HITBOX_HEAD)
							ScanHead(pLocal, Target);
					}
				}

				else
				{
					F::LagRecordMatrixHelper->Set(Target.m_pLagRecord);

					bool bTraceResult = F::AimUtils->TraceEntityBullet(Target.m_pEntity, vLocalPos, Target.m_vPosition);

					F::LagRecordMatrixHelper->Restore();

					if (!bTraceResult)
						continue;
				}

				break;
			}

			case ETFClassIds::CObjectSentrygun:
			case ETFClassIds::CObjectDispenser:
			case ETFClassIds::CObjectTeleporter:
			{
				if (!F::AimUtils->TraceEntityBullet(Target.m_pEntity, vLocalPos, Target.m_vPosition))
				{
					if (!ScanBuilding(pLocal, Target))
						continue;
				}
				
				break;
			}

			case ETFClassIds::CTFGrenadePipebombProjectile:
			{
				if (!F::AimUtils->TraceEntityBullet(Target.m_pEntity, vLocalPos, Target.m_vPosition))
				{
					continue;
				}

				break;
			}

			default: continue;
		}

		Out = Target;
		return true;
	}

	return false;
}

bool CAimbotHitscan::ShouldAim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Target_t &Target)
{
	if (CFG::Aimbot_Hitscan_Aim_Type == 1 && (!IsFiring(pCmd, pWeapon) || !pWeapon->HasPrimaryAmmoForShot()))
		return false;

	if (CFG::Aimbot_Hitscan_Aim_Type == 2)
	{
		int nWeaponID = pWeapon->GetWeaponID();

		if (nWeaponID == TF_WEAPON_SNIPERRIFLE || nWeaponID == TF_WEAPON_SNIPERRIFLE_CLASSIC || nWeaponID == TF_WEAPON_SNIPERRIFLE_DECAP)
		{
			if (!G::bCanPrimaryAttack)
				return false;
		}
	}

	if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN && pWeapon->As<C_TFMinigun>()->m_iWeaponState() == AC_STATE_DRYFIRE)
		return false;

	return true;
}

void CAimbotHitscan::Aim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Vec3 &vAngles)
{
	Vec3 vAngleTo = vAngles - pLocal->m_vecPunchAngle();
	Math::ClampAngles(vAngleTo);

	switch (CFG::Aimbot_Hitscan_Aim_Type)
	{
		case 0:
		{
			pCmd->viewangles = vAngleTo;
			break;
		}

		case 1:
		{
			if (G::bCanPrimaryAttack)
			{
				F::AimUtils->FixMovement(pCmd, vAngleTo);
				pCmd->viewangles = vAngleTo;
				G::bSilentAngles = true;
			}
		
			break;
		}

		case 2:
		{
			Vec3 vDelta = vAngleTo - pCmd->viewangles;
			Math::ClampAngles(vDelta);

			if (vDelta.Length() > 0.0f && CFG::Aimbot_Hitscan_Smoothing)
				pCmd->viewangles += vDelta / CFG::Aimbot_Hitscan_Smoothing;

			break;
		}

		default: break;
	}
}

bool CAimbotHitscan::ShouldFire(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Target_t &Target)
{
	if (!CFG::Aimbot_AutoShoot)
		return false;

	bool bIsMachina = pWeapon->m_iItemDefinitionIndex() == Sniper_m_TheMachina || pWeapon->m_iItemDefinitionIndex() == Sniper_m_ShootingStar;
	bool bCapableOfHeadshot = F::AimUtils->IsWeaponCapableOfHeadshot(pWeapon);
	bool bIsSydneySleeper = pWeapon->m_iItemDefinitionIndex() == Sniper_m_TheSydneySleeper;
	bool bIsSniper = pLocal->m_iClass() == TF_CLASS_SNIPER;

	if (bIsMachina && !pLocal->IsZoomed())
		return false;

	if (CFG::Aimbot_Hitscan_Wait_For_Headshot)
	{
		if (Target.m_pEntity->GetClassId() == ETFClassIds::CTFPlayer && bCapableOfHeadshot && !G::bCanHeadshot)
			return false;
	}

	if (CFG::Aimbot_Hitscan_Wait_For_Charge)
	{
		if (Target.m_pEntity->GetClassId() == ETFClassIds::CTFPlayer && bIsSniper && (bCapableOfHeadshot || bIsSydneySleeper))
		{
			auto pPlayer = Target.m_pEntity->As<C_TFPlayer>();
			auto pSniperRifle = pWeapon->As<C_TFSniperRifle>();

			int nHealth = pPlayer->m_iHealth();
			bool bIsCritBoosted = pLocal->IsCritBoosted();

			if (Target.m_nAimedHitbox == HITBOX_HEAD && !bIsSydneySleeper)
			{
				if (nHealth > 150)
				{
					float flDamage = Math::RemapValClamped(pSniperRifle->m_flChargedDamage(), 0.0f, 150.0f, 0.0f, 450.0f);
					int nDamage = static_cast<int>(flDamage);

					if (nDamage < nHealth && nDamage != 450)
						return false;
				}

				else
				{
					if (!bIsCritBoosted && !G::bCanHeadshot)
						return false;
				}
			}

			else
			{
				if (nHealth > (bIsCritBoosted ? 150 : 50))
				{
					float flMult = pPlayer->IsMarked() ? 1.36f : 1.0f;

					if (bIsCritBoosted)
						flMult = 3.0f;

					float flMax = 150.0f * flMult;
					int nDamage = static_cast<int>(pSniperRifle->m_flChargedDamage() * flMult);

					if (nDamage < pPlayer->m_iHealth() && nDamage != static_cast<int>(flMax))
						return false;
				}
			}
		}
	}

	if (CFG::Aimbot_Hitscan_Minigun_TapFire)
	{
		if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN)
		{
			if (pLocal->GetAbsOrigin().DistTo(Target.m_vPosition) >= 900.0f)
			{
				if ((pLocal->m_nTickBase() * TICK_INTERVAL) - pWeapon->m_flLastFireTime() <= 0.25f)
					return false;
			}
		}
	}

	if ((CFG::Aimbot_Hitscan_Advanced_Smooth_AutoShoot && CFG::Aimbot_Hitscan_Aim_Type == 2))
	{
		Vec3 vForward = {};
		Math::AngleVectors(pCmd->viewangles, &vForward);
		Vec3 vTraceStart = pLocal->GetShootPos();
		Vec3 vTraceEnd = vTraceStart + (vForward * 8192.0f);

		if (Target.m_pEntity->GetClassId() == ETFClassIds::CTFPlayer)
		{
			auto pPlayer = Target.m_pEntity->As<C_TFPlayer>();

			if (!Target.m_pLagRecord)
			{
				int nHitHitbox = -1;

				if (!F::AimUtils->TraceEntityBullet(pPlayer, vTraceStart, vTraceEnd, &nHitHitbox))
					return false;

				if (Target.m_nAimedHitbox == HITBOX_HEAD)
				{
					if (nHitHitbox != HITBOX_HEAD)
						return false;

					if (!Target.m_bWasMultiPointed)
					{
						Vec3 vMins = {}, vMaxs = {}, vCenter = {};
						matrix3x4_t Matrix = {};
						pPlayer->GetHitboxInfo(nHitHitbox, &vCenter, &vMins, &vMaxs, &Matrix);

						vMins *= 0.5f;
						vMaxs *= 0.5f;

						if (!Math::RayToOBB(vTraceStart, vForward, vCenter, vMins, vMaxs, Matrix))
							return false;
					}
				}
			}

			else
			{
				F::LagRecordMatrixHelper->Set(Target.m_pLagRecord);

				int nHitHitbox = -1;

				if (!F::AimUtils->TraceEntityBullet(pPlayer, vTraceStart, vTraceEnd, &nHitHitbox)) {
					F::LagRecordMatrixHelper->Restore();
					return false;
				}

				if (Target.m_nAimedHitbox == HITBOX_HEAD)
				{
					if (nHitHitbox != HITBOX_HEAD) {
						F::LagRecordMatrixHelper->Restore();
						return false;
					}

					Vec3 vMins = {}, vMaxs = {}, vCenter = {};
					SDKUtils::GetHitboxInfoFromMatrix(pPlayer, nHitHitbox, const_cast<matrix3x4_t *>(Target.m_pLagRecord->m_BoneMatrix), &vCenter, &vMins, &vMaxs);

					vMins *= 0.5f;
					vMaxs *= 0.5f;

					if (!Math::RayToOBB(vTraceStart, vForward, vCenter, vMins, vMaxs, *Target.m_pLagRecord->m_BoneMatrix)) {
						F::LagRecordMatrixHelper->Restore();
						return false;
					}
				}

				F::LagRecordMatrixHelper->Restore();
			}
		}

		else
		{
			if (!F::AimUtils->TraceEntityBullet(Target.m_pEntity, vTraceStart, vTraceEnd, nullptr))
				return false;
		}
	}

	return true;
}

void CAimbotHitscan::HandleFire(CUserCmd *pCmd, C_TFWeaponBase *pWeapon)
{
	if (!pWeapon->HasPrimaryAmmoForShot())
		return;
	
	if (pWeapon->GetWeaponID() == TF_WEAPON_SNIPERRIFLE_CLASSIC)
	{
		if (G::nOldButtons & IN_ATTACK)
			pCmd->buttons &= ~IN_ATTACK;

		else pCmd->buttons |= IN_ATTACK;
	}

	else
	{
		pCmd->buttons |= IN_ATTACK;
	}
}

bool CAimbotHitscan::IsFiring(CUserCmd *pCmd, C_TFWeaponBase *pWeapon)
{
	if (!pWeapon->HasPrimaryAmmoForShot())
		return false;

	if (pWeapon->GetWeaponID() == TF_WEAPON_SNIPERRIFLE_CLASSIC)
		return !(pCmd->buttons & IN_ATTACK) && (G::nOldButtons & IN_ATTACK);

	return (pCmd->buttons & IN_ATTACK) && G::bCanPrimaryAttack;
}

void CAimbotHitscan::Run(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon)
{
	if (!CFG::Aimbot_Hitscan_Active)
		return;

	if (CFG::Aimbot_Hitscan_Sort == 0)
		G::flAimbotFOV = CFG::Aimbot_Hitscan_FOV;

	if (Shifting::bShifting && !Shifting::bShiftingWarp)
		return;

	bool is_firing{ IsFiring(pCmd, pWeapon) };

	Target_t Target = {};

	if (GetTarget(pLocal, pWeapon, Target) && Target.m_pEntity)
	{
		G::nTargetIndexEarly = Target.m_pEntity->entindex();

		auto key_down{ H::Input->IsDown(CFG::Aimbot_Key) };

		if (key_down || is_firing)
		{
			G::nTargetIndex = Target.m_pEntity->entindex();

			if (CFG::Aimbot_Hitscan_Auto_Scope
				&& !pLocal->IsZoomed() && pLocal->m_iClass() == TF_CLASS_SNIPER && pWeapon->GetSlot() == WEAPON_SLOT_PRIMARY && G::bCanPrimaryAttack)
			{
				pCmd->buttons |= IN_ATTACK2;
				return;
			}

			if (CFG::Aimbot_AutoShoot && pWeapon->GetWeaponID() == TF_WEAPON_SNIPERRIFLE_CLASSIC)
				pCmd->buttons |= IN_ATTACK;
			
			if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN)
			{
				int nState = pWeapon->As<C_TFMinigun>()->m_iWeaponState();

				if (nState == AC_STATE_IDLE || nState == AC_STATE_STARTFIRING)
					G::bCanPrimaryAttack = false; //hack

				pCmd->buttons |= IN_ATTACK2;
			}

			if (ShouldFire(pCmd, pLocal, pWeapon, Target))
				HandleFire(pCmd, pWeapon);

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
		}
	}
}