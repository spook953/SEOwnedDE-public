#pragma once

#include "../../../SDK/SDK.h"

class CRadar
{
private:
	void Drag();
	bool GetDrawPosition(int &x, int &y, const Vec3 &vWorld);

public:
	void Run();
};

MAKE_SINGLETON_SCOPED(CRadar, Radar, F);