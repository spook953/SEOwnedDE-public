#include "../../SDK/SDK.h"

MAKE_HOOK(
	C_BaseEntity_SetAbsVelocity, Signatures::C_BaseEntity_SetAbsVelocity.Get(),
	void, __fastcall, void *ecx, void *edx, const Vector &vecAbsVelocity)
{
	if (reinterpret_cast<DWORD>(_ReturnAddress()) == Signatures::C_BasePlayer_PostDataUpdate_SetAbsVelocityCall.Get())
	{
		if (auto pBasePlayer = reinterpret_cast<C_TFPlayer *>(ecx))
		{
			if (G::mapVelFixRecords.find(pBasePlayer) != G::mapVelFixRecords.end())
			{
				const auto &Record = G::mapVelFixRecords[pBasePlayer];

				float flSimTimeDelta = pBasePlayer->m_flSimulationTime() - Record.m_flSimulationTime;

				if (flSimTimeDelta > 0.0f)
				{
					Vec3 vOldOrigin = Record.m_vecOrigin;

					int nCurFlags = pBasePlayer->m_fFlags();
					int nOldFlags = Record.m_fFlags;

					if (!(nCurFlags & FL_ONGROUND) && !(nOldFlags & FL_ONGROUND))
					{
						bool bCorrected = false;

						if ((nCurFlags & FL_DUCKING) && !(nOldFlags & FL_DUCKING)) {
							vOldOrigin.z += 20.0f;
							bCorrected = true;
						}

						if (!(nCurFlags & FL_DUCKING) && (nOldFlags & FL_DUCKING)) {
							vOldOrigin.z -= 20.0f;
							bCorrected = true;
						}

						if (bCorrected)
						{
							Vec3 vNewVelocity = vecAbsVelocity;
							vNewVelocity.z = (pBasePlayer->m_vecOrigin().z - vOldOrigin.z) / flSimTimeDelta;
							CALL_ORIGINAL(ecx, edx, vNewVelocity);
							return;
						}
					}
				}
			}
		}
	}

	CALL_ORIGINAL(ecx, edx, vecAbsVelocity);
}