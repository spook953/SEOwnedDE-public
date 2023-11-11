#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CRendering3dView_EnableWorldFog, Signatures::CRendering3dView_EnableWorldFog.Get(),
	void, __fastcall, void *ecx, void *edx)
{
	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		CALL_ORIGINAL(ecx, edx);

		return;
	}

	if (CFG::Visuals_Remove_Fog)
	{
		if (auto pRenderContext = I::MaterialSystem->GetRenderContext())
		{
			const float clr[]{ 0.0f, 0.0f, 0.0f };

			pRenderContext->FogMode(MATERIAL_FOG_LINEAR);
			pRenderContext->FogColor3fv(clr);
			pRenderContext->FogStart(0.0f);
			pRenderContext->FogEnd(0.0f);
			pRenderContext->FogMaxDensity(0.0f);

			return;
		}
	}

	CALL_ORIGINAL(ecx, edx);
}

MAKE_SIGNATURE(CSkyboxView_GetSkyboxFogEnable, "client.dll", "55 8B EC 83 EC 10 57 E8 ? ? ? ? 8B F8 85 FF 74 57", 0);

MAKE_HOOK(
	CSkyboxView_GetSkyboxFogEnable, Signatures::CSkyboxView_GetSkyboxFogEnable.Get(),
	bool, __fastcall, void *ecx, void *edx)
{
	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return CALL_ORIGINAL(ecx, edx);
	}

	if (CFG::Visuals_Remove_Sky_Fog)
	{
		return false;
	}

	return CALL_ORIGINAL(ecx, edx);
}