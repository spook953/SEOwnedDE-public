#pragma once

#include "../../../SDK/SDK.h"

class CMaterials
{
public:
	IMaterial *m_pFlat = nullptr;
	IMaterial *m_pShaded = nullptr;
	IMaterial *m_pGlossy = nullptr;
	IMaterial *m_pGlow = nullptr;
	IMaterial *m_pPlastic = nullptr;
	IMaterialVar *m_pGlowEnvmapTint = nullptr;
	IMaterialVar *m_pGlowSelfillumTint = nullptr;
	IMaterial *m_pFlatNoInvis = nullptr;
	IMaterial *m_pShadedNoInvis = nullptr;

private:
	void Initialize();

	std::map<C_BaseEntity *, bool> m_mapDrawnEntities = {};
	bool m_bRendering = false;
	bool m_bRenderingOriginalMat = false;
	bool m_bCleaningUp = false;

	void DrawEntity(C_BaseEntity *pEntity);

private:
	void RunLagRecords();

public:
	void Run();
	void CleanUp();

	inline bool HasDrawn(C_BaseEntity *pEntity) {
		return m_mapDrawnEntities.find(pEntity) != m_mapDrawnEntities.end();
	}

	inline bool IsRendering() {
		return m_bRendering;
	}

	inline bool IsRenderingOriginalMat() {
		return m_bRenderingOriginalMat;
	}

	inline bool IsUsedMaterial(IMaterial *pMaterial)
	{
		return pMaterial == m_pFlat
			|| pMaterial == m_pShaded
			|| pMaterial == m_pGlossy
			|| pMaterial == m_pGlow
			|| pMaterial == m_pPlastic
			|| pMaterial == m_pFlatNoInvis
			|| pMaterial == m_pShadedNoInvis;
	}

	inline bool IsCleaningUp() { return m_bCleaningUp; }
};

MAKE_SINGLETON_SCOPED(CMaterials, Materials, F);