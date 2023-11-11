#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	KeyValues_SetInt, Signatures::KeyValues_SetInt.Get(),
	void, __fastcall, void *ecx, void *edx, const char *keyName, int value)
{
	CALL_ORIGINAL(ecx, edx, keyName, value);

	if (CFG::Visuals_Reveal_Scoreboard)
	{
		if (reinterpret_cast<DWORD>(_ReturnAddress()) == Signatures::RevealScoreboard_KeyValues_SetInt_Desired.Get()
			&& std::string_view(keyName).find("nemesis") != std::string_view::npos)
		{
			*reinterpret_cast<uintptr_t *>(_AddressOfReturnAddress()) = Signatures::RevealScoreboard_KeyValues_SetInt_Jump.Get();
		}
	}
}