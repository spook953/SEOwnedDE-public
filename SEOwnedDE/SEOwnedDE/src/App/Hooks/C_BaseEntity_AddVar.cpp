#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	C_BaseEntity_AddVar, Signatures::C_BaseEntity_AddVar.Get(),
	void, __fastcall, void *ecx, void *edx, void *data, IInterpolatedVar *watcher, int type, bool bSetup)
{
	if (CFG::Misc_Accuracy_Improvements && watcher)
	{
		auto hash = HASH_RT(watcher->GetDebugName());

		static const auto m_iv_vecVelocity = HASH_CT("C_BaseEntity::m_iv_vecVelocity");
		static const auto m_iv_angEyeAngles = HASH_CT("C_TFPlayer::m_iv_angEyeAngles");
		static const auto m_iv_flPoseParameter = HASH_CT("C_BaseAnimating::m_iv_flPoseParameter");
		static const auto m_iv_flCycle = HASH_CT("C_BaseAnimating::m_iv_flCycle");
		static const auto m_iv_flMaxGroundSpeed = HASH_CT("CMultiPlayerAnimState::m_iv_flMaxGroundSpeed");

		if (hash == m_iv_vecVelocity
			|| hash == m_iv_flPoseParameter
			|| hash == m_iv_flCycle
			|| hash == m_iv_flMaxGroundSpeed)
			return;

		if (ecx != H::Entities->GetLocal())
		{
			if (hash == m_iv_angEyeAngles)
				return;
		}
	}

	CALL_ORIGINAL(ecx, edx, data, watcher, type, bSetup);
}

MAKE_HOOK(
	C_BaseEntity_EstimateAbsVelocity, Signatures::C_BaseEntity_EstimateAbsVelocity.Get(),
	void, __fastcall, void *ecx, void *edx, Vector &vel)
{
	if (CFG::Misc_Accuracy_Improvements)
	{
		if (auto pEntity = reinterpret_cast<C_BaseEntity *>(ecx))
		{
			if (pEntity->GetClassId() == ETFClassIds::CTFPlayer)
			{
				if (auto pPlayer = pEntity->As<C_TFPlayer>()) {
					vel = pPlayer->m_vecVelocity();
					return;
				}
			}
		}
	}

	CALL_ORIGINAL(ecx, edx, vel);
}