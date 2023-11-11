#include "../../SDK/SDK.h"

MAKE_HOOK(
	CTFGameMovement_ProcessMovement, Signatures::CTFGameMovement_ProcessMovement.Get(),
	void, __fastcall, void *ecx, void *edx, C_BasePlayer *pBasePlayer, CMoveData *pMove)
{
	I::TFGameMovement = ecx;

	CALL_ORIGINAL(ecx, edx, pBasePlayer, pMove);
}