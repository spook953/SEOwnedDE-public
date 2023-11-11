#include "../../SDK/SDK.h"

#include "../Features/CFG.h"
#include "../Features/seed_pred/seed_pred.hpp"

MAKE_HOOK(
	CUserMessages_DispatchUserMessage, Signatures::CUserMessages_DispatchUserMessage.Get(),
	bool, __fastcall, void *ecx, void *edx, int msg_type, bf_read &msg_data)
{
	if (msg_type == 10 && CFG::Visuals_Remove_Screen_Shake)
	{
		return true;
	}

	if (msg_type == 11 && CFG::Visuals_Remove_Screen_Fade)
	{
		return true;
	}

	if (msg_type == 5 && seed_pred::parsePlayerPerf(msg_data))
	{
		return true;
	}

	return CALL_ORIGINAL(ecx, edx, msg_type, msg_data);
}