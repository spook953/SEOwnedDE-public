#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	C_TFPlayer_AvoidPlayers, Memory::RelToAbs(Signatures::C_TFPlayer_AvoidPlayers.Get()),
	void, __fastcall, void *ecx, void *edx, CUserCmd *pCmd)
{
	if (CFG::Misc_No_Push)
		return;

	CALL_ORIGINAL(ecx, edx, pCmd);
}