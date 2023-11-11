#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	C_BaseEntity_ResetLatched, Signatures::C_BaseEntity_ResetLatched.Get(),
	void, __fastcall, void *ecx, void *edx)
{
	if (CFG::Misc_Pred_Error_Jitter_Fix)
	{
		return;
	}

	CALL_ORIGINAL(ecx, edx);
}