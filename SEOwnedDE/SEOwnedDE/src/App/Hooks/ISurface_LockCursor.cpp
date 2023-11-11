#include "../../SDK/SDK.h"

#include "../Features/Menu/Menu.h"

MAKE_HOOK(
	ISurface_LockCursor, Memory::GetVFunc(I::MatSystemSurface, 62),
	void, __fastcall, void *ecx, void *edx)
{
	F::Menu->IsOpen() ? I::MatSystemSurface->UnlockCursor() : CALL_ORIGINAL(ecx, edx);
}