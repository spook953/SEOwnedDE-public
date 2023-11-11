#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CBasePlayer_CalcPlayerView, Signatures::CBasePlayer_CalcPlayerView.Get(),
	void, __fastcall, void *ecx, void *edx, Vector &eyeOrigin, QAngle &eyeAngles, float &fov)
{
	if (CFG::Visuals_Remove_Punch)
	{
		if (auto pBasePlayer = reinterpret_cast<C_BasePlayer *>(ecx))
		{
			Vec3 vOldPunch = pBasePlayer->m_vecPunchAngle();
			pBasePlayer->m_vecPunchAngle() = {};
			CALL_ORIGINAL(ecx, edx, eyeOrigin, eyeAngles, fov);
			pBasePlayer->m_vecPunchAngle() = vOldPunch;
			return;
		}
	}

	CALL_ORIGINAL(ecx, edx, eyeOrigin, eyeAngles, fov);
}