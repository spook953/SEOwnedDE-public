#pragma once

#include "interface.h"

class IUniformRandomStream
{
public:
	virtual void SetSeed(int iSeed) = 0;
	virtual float RandomFloat(float flMinVal = 0.0f, float flMaxVal = 1.0f) = 0;
	virtual int	RandomInt(int iMinVal, int iMaxVal) = 0;
	virtual float RandomFloatExp(float flMinVal = 0.0f, float flMaxVal = 1.0f, float flExponent = 1.0f) = 0;
};

MAKE_INTERFACE_SIGNATURE(
	IUniformRandomStream, UniformRandomStream,
	"client.dll", "8B 0D ? ? ? ? 83 EC 08 D9 05 ? ? ? ? 8B 01 D9 5C 24 04 D9 05 ? ? ? ? D9 1C 24 8B 40 04 FF D0 D8 4D A0 D9 7D FA 0F B7 45 FA",
	2, 2
);