#pragma once

#include "../../../SDK/SDK.h"

class CRapidFire
{
private:
	CUserCmd m_ShiftCmd = {};
	bool m_bShiftSilentAngles = false;
	bool m_bSetCommand = false;

public:
	inline bool GetShiftSilentAngles() { return m_bShiftSilentAngles; }

private:
	bool ShouldStart(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon);

public:
	void Run(CUserCmd *pCmd, bool *pSendPacket);
	bool ShouldExitCreateMove(CUserCmd *pCmd);
	bool IsWeaponSupported(C_TFWeaponBase *pWeapon);
};

MAKE_SINGLETON_SCOPED(CRapidFire, RapidFire, F);