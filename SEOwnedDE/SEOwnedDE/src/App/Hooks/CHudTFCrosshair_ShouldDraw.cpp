#include "../../SDK/SDK.h"

MAKE_HOOK(
	CHudTFCrosshair_ShouldDraw, Signatures::CHudTFCrosshair_ShouldDraw.Get(),
	bool, __fastcall, void *ecx, void *edx)
{
	if (G::bStartedFakeTaunt)
		return G::CHudTFCrosshair_ShouldDraw_Result = false;

	return G::CHudTFCrosshair_ShouldDraw_Result = CALL_ORIGINAL(ecx, edx);
}