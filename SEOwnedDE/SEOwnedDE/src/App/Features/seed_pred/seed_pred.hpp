#pragma once

#include "../../../SDK/SDK.h"

namespace seed_pred
{
	void askForPlayerPerf();
	bool parsePlayerPerf(bf_read &msg_data);
	int getSeed();
	void reset();
	void adjustAngles(CUserCmd *cmd);
	void paint();
}