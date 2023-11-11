#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	R_LoadSkys, Memory::RelToAbs(Signatures::R_LoadSkys.Get()),
	void, __cdecl)
{
	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		CALL_ORIGINAL();

		return;
	}

	auto custom{ CFG::Visuals_Custom_Skybox_Texture_Name };

	if (!custom.empty())
	{
		static auto sv_skyname{ I::CVar->FindVar("sv_skyname") };

		std::string original{ sv_skyname->GetString() };

		sv_skyname->SetValue(custom.c_str());

		CALL_ORIGINAL();

		sv_skyname->SetValue(original.c_str());

		return;
	}

	CALL_ORIGINAL();
}