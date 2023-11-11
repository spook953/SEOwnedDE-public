#include "../../SDK/SDK.h"

#include "../Features/MovementSimulation/MovementSimulation.h"

MAKE_HOOK(
	CTFGameMovement_PlayerMove, Signatures::CTFGameMovement_PlayerMove.Get(),
	void, __fastcall, void *ecx, void *edx)
{
	if (F::MovementSimulation->IsRunning()) {
		reinterpret_cast<void(__thiscall *)(void *)>(Memory::RelToAbs(Signatures::CGameMovement_PlayerMove.Get()))(ecx);
		return;
	}

	CALL_ORIGINAL(ecx, edx);
}