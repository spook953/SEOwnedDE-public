#include "../../SDK/SDK.h"

#include "../Features/SpyCamera/SpyCamera.h"
#include "../Features/CFG.h"

MAKE_HOOK(
	CViewRender_RenderView, Signatures::CViewRender_RenderView.Get(),
	void, __fastcall, void *ecx, void *edx, const CViewSetup &view, int nClearFlags, int whatToDraw)
{
	I::ViewRender = reinterpret_cast<IViewRender *>(ecx);

	CALL_ORIGINAL(ecx, edx, view, nClearFlags, whatToDraw);

	G::View = view;
}