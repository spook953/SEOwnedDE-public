#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	C_BaseEntity_InterpolateServerEntities, Signatures::C_BaseEntity_InterpolateServerEntities.Get(),
	void, __cdecl)
{
	if (CFG::Misc_Accuracy_Improvements)
	{
		static auto cl_extrapolate = I::CVar->FindVar("cl_extrapolate");

		if (cl_extrapolate && cl_extrapolate->GetInt())
			cl_extrapolate->SetValue(0);
	}

	CALL_ORIGINAL();
}