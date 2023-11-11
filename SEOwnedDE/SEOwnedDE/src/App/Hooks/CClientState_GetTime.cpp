#include "../../SDK/SDK.h"

//MAKE_HOOK(
//	CClientState_GetTime, Signatures::CClientState_GetTime.Get(),
//	float, __fastcall, void *ecx, void *edx)
//{
//	if (reinterpret_cast<DWORD>(_ReturnAddress()) == Signatures::CL_FireEvents_CClientState_GetTime_Call.Get())
//		return FLT_MAX;
//
//	return CALL_ORIGINAL(ecx, edx);
//}