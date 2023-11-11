#pragma once

#include "../../../SDK/SDK.h"

namespace auto_vaccinator
{
	void run(CUserCmd *cmd);
	void processPlayerHurt(IGameEvent *event);
	void preventReload(CUserCmd *cmd);
}