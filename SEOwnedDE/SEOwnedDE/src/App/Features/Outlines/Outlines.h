#pragma once

#include "../../../SDK/SDK.h"

class COutlines
{
private:
	IMaterial *m_pMatGlowColor = nullptr, *m_pMatHaloAddToScreen = nullptr;
	ITexture *m_pRtFullFrame = nullptr, *m_pRenderBuffer0 = nullptr, *m_pRenderBuffer1 = nullptr;
	IMaterial *m_pMatBlurX = nullptr, *m_pMatBlurY = nullptr;
	IMaterialVar *m_pBloomAmount = nullptr;

	void Initialize();

	std::map<C_BaseEntity *, bool> m_mapDrawnEntities = {};
	bool m_bRendering = false;
	bool m_bRenderingOutlines = false;
	bool m_bCleaningUp = false;

	void DrawEntity(C_BaseEntity *pEntity, bool bModel);

private:
	struct OutlineEntity_t
	{
		C_BaseEntity *m_pEntity = nullptr;
		Color_t m_Color = {};
		float m_flAlpha = 0.0f;
	};

	std::vector<OutlineEntity_t> m_vecOutlineEntities = {};

public:
	void RunModels();
	void Run();
	void CleanUp();
	void SetModelStencil(IMatRenderContext *pRenderContext);

	inline bool HasDrawn(C_BaseEntity *pEntity) {
		return m_mapDrawnEntities.find(pEntity) != m_mapDrawnEntities.end();
	}

	inline bool IsRendering() {
		return m_bRendering;
	}

	inline bool IsRenderingOutlines() {
		return m_bRenderingOutlines;
	}

	inline bool IsUsedMaterial(IMaterial *pMaterial)
	{
		return pMaterial == m_pMatGlowColor
			|| pMaterial == m_pMatBlurX
			|| pMaterial == m_pMatBlurY
			|| pMaterial == m_pMatHaloAddToScreen;
	}

	inline bool IsCleaningUp() { return m_bCleaningUp; }
};

MAKE_SINGLETON_SCOPED(COutlines, Outlines, F);