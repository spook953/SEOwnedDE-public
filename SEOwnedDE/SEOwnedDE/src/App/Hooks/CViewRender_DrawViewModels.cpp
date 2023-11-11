#include "../../SDK/SDK.h"

#include "../Features/Outlines/Outlines.h"
#include "../Features/Paint/Paint.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CViewRender_DrawViewModels, Signatures::CViewRender_DrawViewModels.Get(),
	void, __fastcall, void *ecx, void *edx, const CViewSetup &viewRender, bool drawViewmodel)
{
	G::in_CViewRender_DrawViewModels = true;

	CALL_ORIGINAL(ecx, edx, viewRender, drawViewmodel);

	G::in_CViewRender_DrawViewModels = false;

	F::Outlines->Run();
	F::Paint->Run();
}