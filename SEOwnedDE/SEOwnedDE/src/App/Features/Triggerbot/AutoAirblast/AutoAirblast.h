#pragma once

#include "../../../../SDK/SDK.h"

class CAutoAirblast
{
public:
	void Run(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, CUserCmd *pCmd);
};

MAKE_SINGLETON_SCOPED(CAutoAirblast, AutoAirblast, F);