#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CStudioRender_DrawModelStaticProp, Memory::GetVFunc(I::StudioRender, 30),
	void, __fastcall, void *ecx, void *edx, const DrawModelInfo_t &drawInfo, const matrix3x4_t &modelToWorld, int flags)
{
	if (CFG::Visuals_Distance_Prop_Alpha)
	{
		if (auto pLocal = H::Entities->GetLocal())
		{
			Vec3 vOrigin = { modelToWorld[0][3], modelToWorld[1][3], modelToWorld[2][3] };

			float flDistance = pLocal->m_vecOrigin().DistTo(vOrigin);

			float flAlpha = 1.0f;

			if (flDistance > 100.0f)
				flAlpha = Math::RemapValClamped(pLocal->m_vecOrigin().DistTo(vOrigin), 100.0f, 200.0f, 0.15f, 1.0f);

			else flAlpha = 0.15f;

			I::StudioRender->SetAlphaModulation(flAlpha);
		}
	}

	if (CFG::Visuals_World_Modulation_Mode == 0)
	{
		if (auto flNightMode = CFG::Visuals_Night_Mode)
		{
			auto col{ static_cast<byte>(Math::RemapValClamped(flNightMode, 0.0f, 100.0f, 255.0f, 50.0f)) };

			I::StudioRender->SetColorModulation({ col, col, col, static_cast<byte>(255) });
		}
	}

	else
	{
		I::StudioRender->SetColorModulation(CFG::Color_Props);
	}

	CALL_ORIGINAL(ecx, edx, drawInfo, modelToWorld, flags);
}