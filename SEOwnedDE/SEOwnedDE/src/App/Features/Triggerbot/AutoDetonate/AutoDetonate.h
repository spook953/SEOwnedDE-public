#pragma once

#include "../../../../SDK/SDK.h"

class CAutoDetonate
{
public:
	void Run(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, CUserCmd *pCmd);
};

MAKE_SINGLETON_SCOPED(CAutoDetonate, AutoDetonate, F);