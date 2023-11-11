#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	C_TFDroppedWeapon_OnDataChanged, Signatures::C_TFDroppedWeapon_OnDataChanged.Get(),
	void, __fastcall, void *ecx, void *edx, DataUpdateType_t updateType)
{
	if (CFG::Visuals_Disable_Dropped_Weapons)
		return;

	CALL_ORIGINAL(ecx, edx, updateType);
}

MAKE_HOOK(
	C_TFDroppedWeapon_ClientThink, Signatures::C_TFDroppedWeapon_ClientThink.Get(),
	void, __fastcall, void *ecx, void *edx)
{
	if (CFG::Visuals_Disable_Dropped_Weapons)
		return;

	CALL_ORIGINAL(ecx, edx);
}

MAKE_HOOK(
	C_TFDroppedWeapon_OnInternalDrawModel, Signatures::C_TFDroppedWeapon_OnInternalDrawModel.Get(),
	bool, __fastcall, void *ecx, void *edx, void *pInfo)
{
	if (CFG::Visuals_Disable_Dropped_Weapons)
		return false;

	return CALL_ORIGINAL(ecx, edx, pInfo);
}