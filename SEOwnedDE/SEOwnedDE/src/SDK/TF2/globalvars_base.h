#pragma once
#include "interface.h"

class CSaveRestoreData;

class CGlobalVarsBase
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxClients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
	int simTicksThisFrame;
	int network_protocol;
	CSaveRestoreData *pSaveData;
	bool m_bClient;
	int nTimestampNetworkingBase;
	int nTimestampRandomizeWindow;
};

MAKE_INTERFACE_SIGNATURE(CGlobalVarsBase, GlobalVars, "engine.dll", "A1 ? ? ? ? 8B 11 68", 8, 1);