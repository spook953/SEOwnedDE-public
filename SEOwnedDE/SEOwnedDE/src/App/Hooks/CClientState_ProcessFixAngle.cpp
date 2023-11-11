#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CClientState_ProcessFixAngle, Signatures::CClientState_ProcessFixAngle.Get(),
	bool, __fastcall, void *ecx, void *edx, void *msg)
{
	if (CFG::Misc_Prevent_Server_Angle_Change)
	{
		return true;
	}

	return CALL_ORIGINAL(ecx, edx, msg);
}