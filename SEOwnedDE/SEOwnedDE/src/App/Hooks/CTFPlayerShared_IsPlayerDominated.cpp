#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CTFPlayerShared_IsPlayerDominated, Signatures::CTFPlayerShared_IsPlayerDominated.Get(),
	bool, __fastcall, void *ecx, void *edx, int index)
{
	bool bResult = CALL_ORIGINAL(ecx, edx, index);

	if (!bResult)
	{
		if (CFG::Visuals_Reveal_Scoreboard)
		{
			if (reinterpret_cast<DWORD>(_ReturnAddress()) == Signatures::RevealScoreboard_CTFPlayerShared_IsPlayerDominated_Desired.Get())
				*reinterpret_cast<uintptr_t *>(_AddressOfReturnAddress()) = Signatures::RevealScoreboard_CTFPlayerShared_IsPlayerDominated_Jump.Get();
		}
	}

	return bResult;
}