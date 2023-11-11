#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	C_TFPlayer_BRenderAsZombie, Signatures::C_TFPlayer_BRenderAsZombie.Get(),
	bool, __fastcall, void *ecx, void *edx, bool bWeaponsCheck)
{
	if (CFG::Visuals_Ragdolls_Active && CFG::Visuals_Ragdolls_No_Gib)
	{
		if (reinterpret_cast<DWORD>(_ReturnAddress()) == Signatures::C_TFRagdoll_CreateTFRagdoll_C_TFPlayer_BRenderAsZombie_Call.Get())
			return true;
	}

	return CALL_ORIGINAL(ecx, edx, bWeaponsCheck);
}