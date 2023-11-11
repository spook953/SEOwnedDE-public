#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	C_TFPlayer_UpdateClientSideAnimation, Signatures::C_TFPlayer_UpdateClientSideAnimation.Get(),
	void, __fastcall, void *ecx, void *edx)
{
	if (CFG::Misc_Accuracy_Improvements)
	{
		if (auto pLocal = H::Entities->GetLocal())
		{
			if (ecx == pLocal)
			{
				if (!pLocal->InCond(TF_COND_HALLOWEEN_KART))
				{
					if (auto pWeapon = H::Entities->GetWeapon())
						pWeapon->UpdateAllViewmodelAddons(); //credits: KGB

					return;
				}

				else
				{
					CALL_ORIGINAL(ecx, edx);
				}
			}
		}

		if (!G::bUpdatingAnims)
			return;
	}

	CALL_ORIGINAL(ecx, edx);
}