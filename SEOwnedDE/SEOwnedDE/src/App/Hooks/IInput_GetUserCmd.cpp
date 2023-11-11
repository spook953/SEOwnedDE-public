#include "../../SDK/SDK.h"

MAKE_HOOK(
	IInput_GetUserCmd, Memory::GetVFunc(I::Input, 8),
	CUserCmd *, __fastcall, void *ecx, void *edx, int sequence_number)
{
	return &I::Input->GetCommands()[sequence_number % 90];
}