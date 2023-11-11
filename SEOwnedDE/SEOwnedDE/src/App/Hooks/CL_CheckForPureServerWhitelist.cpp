#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CL_CheckForPureServerWhitelist, Signatures::CL_CheckForPureServerWhitelist.Get(),
	void, __cdecl, void **pFilesToReload)
{
	if (CFG::Misc_Pure_Bypass)
		return;

	CALL_ORIGINAL(pFilesToReload);
}