#include "../../SDK/SDK.h"

MAKE_HOOK(
	C_TFWeapon_ShouldDraw,
	Signatures::C_TFWeapon_ShouldDraw.Get(),
	bool,
	__fastcall,
	void *ecx, void *edx)
{
	if (C_TFWeaponBase *weapon{ reinterpret_cast<C_TFWeaponBase *>(reinterpret_cast<uintptr_t>(ecx) - 4) })
	{
		if (weapon->GetWeaponID() == TF_WEAPON_MEDIGUN)
		{
			if (C_TFPlayer *owner{ reinterpret_cast<C_TFPlayer *>(weapon->m_hOwner().Get()) })
			{
				if (owner->m_hActiveWeapon().Get() == weapon && !owner->InCond(TF_COND_TAUNTING))
				{
					return true;
				}
			}
		}
	}

	return CALL_ORIGINAL(ecx, edx);
}