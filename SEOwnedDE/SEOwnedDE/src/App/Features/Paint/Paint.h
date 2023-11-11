#pragma once

#include "../../../SDK/SDK.h"

class CPaint
{
private:
	IMaterial *m_pMatGlowColor = nullptr, *m_pMatHaloAddToScreen = nullptr;
	ITexture *m_pRtFullFrame = nullptr, *m_pRenderBuffer0 = nullptr, *m_pRenderBuffer1 = nullptr;
	IMaterial *m_pMatBlurX = nullptr, *m_pMatBlurY = nullptr;
	IMaterialVar *m_pBloomAmount = nullptr;

	void Initialize();

private:
	struct PaintRecord_t
	{
		Vec3 m_vPosition = {};
		float m_flTimeAdded = 0.0f;
	};

	std::map<int, std::vector<PaintRecord_t>> m_mapPositions = {};

public:
	void Run();
	void CleanUp();
};

MAKE_SINGLETON_SCOPED(CPaint, Paint, F);