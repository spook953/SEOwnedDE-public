//#include "../../SDK/SDK.h"
//
//#include "../Features/Materials/Materials.h"
//#include "../Features/Outlines/Outlines.h"
//
//MAKE_HOOK(
//	CMaterial_Uncache, Signatures::CMaterial_Uncache.Get(),
//	void, __fastcall, void *ecx, void *edx, bool bPreserveVars)
//{
//	if (auto pMaterial = reinterpret_cast<IMaterial *>(reinterpret_cast<DWORD>(ecx) + 0x90))
//	{
//		if (F::Materials->IsUsedMaterial(pMaterial) && !F::Materials->IsCleaningUp())
//			return;
//
//		if (F::Outlines->IsUsedMaterial(pMaterial) && !F::Outlines->IsCleaningUp())
//			return;
//	}
//
//	CALL_ORIGINAL(ecx, edx, bPreserveVars);
//}