#pragma once

#include "../../../SDK/SDK.h"

class CMisc
{
public:
	void Bunnyhop(CUserCmd *pCmd);
	void AutoStrafer(CUserCmd *pCmd);
	void NoiseMakerSpam();
	void Crits(CUserCmd *pCmd);
	void FastStop(CUserCmd *pCmd);

	void autoRocketJump(CUserCmd *cmd);
	void autoDisguise(CUserCmd *cmd);
	void autoMedigun(CUserCmd *cmd);
	void movementLock(CUserCmd *cmd);
	void mvmInstaRespawn();
};

MAKE_SINGLETON_SCOPED(CMisc, Misc, F);