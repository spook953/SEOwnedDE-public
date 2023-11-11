#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CSequenceTransitioner_CheckForSequenceChange, Memory::RelToAbs(Signatures::CSequenceTransitioner_CheckForSequenceChange.Get()),
	void, __fastcall, void *ecx, void *edx, CStudioHdr *hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate)
{
	if (CFG::Misc_Accuracy_Improvements)
		bInterpolate = false;

	CALL_ORIGINAL(ecx, edx, hdr, nCurSequence, bForceNewSequence, bInterpolate);
}