#pragma once

#include "../../../SDK/SDK.h"

class CTriggerbot
{
public:
	void Run(CUserCmd *pCmd);
};

MAKE_SINGLETON_SCOPED(CTriggerbot, Triggerbot, F);