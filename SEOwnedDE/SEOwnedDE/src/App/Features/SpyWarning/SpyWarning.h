#pragma once

#include "../../../SDK/SDK.h"

class CSpyWarning
{
public:
	void Run();
};

MAKE_SINGLETON_SCOPED(CSpyWarning, SpyWarning, F);