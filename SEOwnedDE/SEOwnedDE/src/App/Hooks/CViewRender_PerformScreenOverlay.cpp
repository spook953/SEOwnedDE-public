#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CViewRender_PerformScreenOverlay, Memory::RelToAbs(Signatures::CViewRender_PerformScreenOverlay.Get()),
	void, __fastcall, void *ecx, void *edx, int x, int y, int w, int h)
{
	if (CFG::Visuals_Remove_Screen_Overlay)
	{
		auto ShouldDoOverrides = [&]()
		{
			if (CFG::Visuals_Removals_Mode == 1)
			{
				if (auto pLocal = H::Entities->GetLocal())
				{
					if (pLocal->m_iObserverMode() == OBS_MODE_IN_EYE)
					{
						return false;
					}
				}
			}

			return true;
		};

		if (ShouldDoOverrides())
		{
			return;
		}
	}

	CALL_ORIGINAL(ecx, edx, x, y, w, h);
}