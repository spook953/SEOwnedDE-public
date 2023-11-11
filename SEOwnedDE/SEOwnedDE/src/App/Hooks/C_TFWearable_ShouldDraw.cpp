#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	C_TFWearable_ShouldDraw, Signatures::C_TFWearable_ShouldDraw.Get(),
	bool, __fastcall, void *ecx, void *edx)
{
	if (CFG::Visuals_Disable_Wearables)
		return false;

	return CALL_ORIGINAL(ecx, edx);
}

MAKE_HOOK(
	C_TFWearable_InternalDrawModel, Signatures::C_TFWearable_InternalDrawModel.Get(),
	int, __fastcall, void *ecx, void *edx, int flags)
{
	if (CFG::Visuals_Disable_Wearables)
		return 0;

	return CALL_ORIGINAL(ecx, edx, flags);
}