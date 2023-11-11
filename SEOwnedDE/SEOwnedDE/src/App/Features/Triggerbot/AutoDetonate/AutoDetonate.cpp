#include "AutoDetonate.h"

#include "../../CFG.h"
#include "../../AimUtils/AimUtils.h"

void CAutoDetonate::Run(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, CUserCmd *pCmd)
{
	if (!CFG::Triggerbot_AutoDetonate_Active)
		return;

	if (pLocal->m_iClass() != TF_CLASS_DEMOMAN)
		return;

	for (auto pEntity : H::Entities->GetGroup(EEntGroup::PROJECTILES_LOCAL_STICKIES))
	{
		if (!pEntity)
			continue;

		auto pSticky = pEntity->As<C_TFGrenadePipebombProjectile>();

		if (!pSticky || pSticky->m_iType() == TF_GL_MODE_REMOTE_DETONATE_PRACTICE)
			continue;

		if (I::GlobalVars->curtime < pSticky->m_flCreationTime() + SDKUtils::AttribHookValue(0.8f, "sticky_arm_time", pLocal))
			continue;

		float flRadius = pSticky->m_bTouched() ? 150.0f : 100.0f;
		
		bool bDetonated = false;

		if (CFG::Triggerbot_AutoDetonate_Target_Players)
		{
			for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ENEMIES))
			{
				if (!pEntity)
					continue;

				auto pPlayer = pEntity->As<C_TFPlayer>();

				if (pPlayer->deadflag() || pPlayer->InCond(TF_COND_HALLOWEEN_GHOST_MODE))
					continue;

				if (CFG::Triggerbot_AutoDetonate_Ignore_Friends && pPlayer->IsPlayerOnSteamFriendsList())
					continue;

				if (CFG::Triggerbot_AutoDetonate_Ignore_Invisible && pPlayer->IsInvisible())
					continue;

				if (CFG::Triggerbot_AutoDetonate_Ignore_Invulnerable && pPlayer->IsInvulnerable())
					continue;

				if (pSticky->GetCenter().DistTo(pPlayer->GetCenter()) < flRadius)
				{
					if (F::AimUtils->TraceEntityAutoDet(pPlayer, pSticky->GetCenter(), pPlayer->GetCenter()))
					{
						if (pSticky->m_bDefensiveBomb()) {
							Vec3 vAngle = Math::CalcAngle(pLocal->GetShootPos(), pSticky->GetCenter());
							F::AimUtils->FixMovement(pCmd, vAngle);
							pCmd->viewangles = vAngle;
							G::bSilentAngles = true;
						}

						pCmd->buttons |= IN_ATTACK2;
						bDetonated = true;
						break;
					}
				}
			}
		}

		if (CFG::Triggerbot_AutoDetonate_Target_Buildings)
		{
			for (auto pEntity : H::Entities->GetGroup(EEntGroup::BUILDINGS_ENEMIES))
			{
				if (!pEntity)
					continue;

				if (pSticky->GetCenter().DistTo(pEntity->GetCenter()) < flRadius)
				{
					if (F::AimUtils->TraceEntityAutoDet(pEntity, pSticky->GetCenter(), pEntity->GetCenter()))
					{
						if (pSticky->m_bDefensiveBomb()) {
							Vec3 vAngle = Math::CalcAngle(pLocal->GetShootPos(), pSticky->GetCenter());
							F::AimUtils->FixMovement(pCmd, vAngle);
							pCmd->viewangles = vAngle;
							G::bSilentAngles = true;
						}

						pCmd->buttons |= IN_ATTACK2;
						bDetonated = true;
						break;
					}
				}
			}
		}

		if (bDetonated)
			break;
	}
}