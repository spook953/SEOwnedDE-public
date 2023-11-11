#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

typedef union
{
	float m128_f32[4];
	uint32_t m128_u32[4];
} fltx4;

struct SpriteRenderInfo_t
{
	size_t m_nXYZStride{};
	fltx4 *m_pXYZ{};
	size_t m_nRotStride{};
	fltx4 *m_pRot{};
	size_t m_nYawStride{};
	fltx4 *m_pYaw{};
	size_t m_nRGBStride{};
	fltx4 *m_pRGB{};
	size_t m_nCreationTimeStride{};
	fltx4 *m_pCreationTimeStamp{};
	size_t m_nSequenceStride{};
	fltx4 *m_pSequenceNumber{};
	size_t m_nSequence1Stride{};
	fltx4 *m_pSequence1Number{};
	float m_flAgeScale{};
	float m_flAgeScale2{};
	void *m_pSheet{};
	int m_nVertexOffset{};
	void *m_pParticles{};
};

MAKE_HOOK(
	C_OP_RenderSprites_RenderSpriteCard, Memory::RelToAbs(Signatures::C_OP_RenderSprites_RenderSpriteCard.Get()),
	void, __fastcall, void *ecx, void *edx, void *meshBuilder, void *pCtx, SpriteRenderInfo_t &info, int hParticle, void *pSortList, void *pCamera)
{
	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		CALL_ORIGINAL(ecx, edx, meshBuilder, pCtx, info, hParticle, pSortList, pCamera);

		return;
	}

	if (auto mode{ CFG::Visuals_Particles_Mode })
	{
		Color_t color{};

		if (mode == 1)
		{
			color = CFG::Color_Particles;
		}

		else if (mode == 2)
		{
			auto rainbow = [&]()
			{
				float rate{ CFG::Visuals_Particles_Rainbow_Rate };

				int r{ std::lround(std::cos(I::GlobalVars->realtime * rate + 0.0f) * 127.5f + 127.5f) };
				int g{ std::lround(std::cos(I::GlobalVars->realtime * rate + 2.0f) * 127.5f + 127.5f) };
				int b{ std::lround(std::cos(I::GlobalVars->realtime * rate + 4.0f) * 127.5f + 127.5f) };

				return Color_t{ static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b), static_cast<unsigned char>(255) };
			};

			color = rainbow();
		}

		info.m_pRGB[((hParticle / 4) * info.m_nRGBStride) + 0].m128_f32[hParticle & 0x3] = ColorUtils::ToFloat(color.r);
		info.m_pRGB[((hParticle / 4) * info.m_nRGBStride) + 1].m128_f32[hParticle & 0x3] = ColorUtils::ToFloat(color.g);
		info.m_pRGB[((hParticle / 4) * info.m_nRGBStride) + 2].m128_f32[hParticle & 0x3] = ColorUtils::ToFloat(color.b);
	}

	CALL_ORIGINAL(ecx, edx, meshBuilder, pCtx, info, hParticle, pSortList, pCamera);
}