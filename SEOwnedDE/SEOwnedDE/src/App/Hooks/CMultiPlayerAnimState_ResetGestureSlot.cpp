#include "../../SDK/SDK.h"

MAKE_HOOK(
	CMultiPlayerAnimState_ResetGestureSlot, Signatures::CMultiPlayerAnimState_ResetGestureSlot.Get(),
	void, __fastcall, void *ecx, void *edx, int iGestureSlot)
{
	if (auto pLocal = H::Entities->GetLocal())
	{
		if (auto pAnimState = reinterpret_cast<CMultiPlayerAnimState *>(ecx))
		{
			if (pAnimState->m_pEntity == pLocal && iGestureSlot == GESTURE_SLOT_VCD && G::bStartedFakeTaunt)
				return;
		}
	}

	CALL_ORIGINAL(ecx, edx, iGestureSlot);
}