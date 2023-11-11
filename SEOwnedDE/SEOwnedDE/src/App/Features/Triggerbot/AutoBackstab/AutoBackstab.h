#pragma once

#include "../../../../SDK/SDK.h"

class CAutoBackstab
{
public:
	void Run(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, CUserCmd *pCmd);
};

MAKE_SINGLETON_SCOPED(CAutoBackstab, AutoBackstab, F);