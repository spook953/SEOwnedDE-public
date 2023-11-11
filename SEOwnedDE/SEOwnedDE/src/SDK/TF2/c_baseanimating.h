#pragma once
#include "c_baseentity.h"
#include "ivmodelinfo.h"
#include "globalvars_base.h"
#include "studio.h"

class C_BaseAnimating : public C_BaseEntity
{
public:
	NETVAR(m_nSequence, int, "CBaseAnimating", "m_nSequence");
	NETVAR(m_nForceBone, int, "CBaseAnimating", "m_nForceBone");
	NETVAR(m_vecForce, Vec3, "CBaseAnimating", "m_vecForce");
	NETVAR(m_nSkin, int, "CBaseAnimating", "m_nSkin");
	NETVAR(m_nBody, int, "CBaseAnimating", "m_nBody");
	NETVAR(m_nHitboxSet, int, "CBaseAnimating", "m_nHitboxSet");
	NETVAR(m_flModelScale, float, "CBaseAnimating", "m_flModelScale");
	NETVAR(m_flModelWidthScale, float, "CBaseAnimating", "m_flModelWidthScale");
	NETVAR(m_flPlaybackRate, float, "CBaseAnimating", "m_flPlaybackRate");
	NETVAR(m_flEncodedController, void *, "CBaseAnimating", "m_flEncodedController");
	NETVAR(m_bClientSideAnimation, bool, "CBaseAnimating", "m_bClientSideAnimation");
	NETVAR(m_bClientSideFrameReset, bool, "CBaseAnimating", "m_bClientSideFrameReset");
	NETVAR(m_nNewSequenceParity, int, "CBaseAnimating", "m_nNewSequenceParity");
	NETVAR(m_nResetEventsParity, int, "CBaseAnimating", "m_nResetEventsParity");
	NETVAR(m_nMuzzleFlashParity, int, "CBaseAnimating", "m_nMuzzleFlashParity");
	NETVAR(m_hLightingOrigin, EHANDLE, "CBaseAnimating", "m_hLightingOrigin");
	NETVAR(m_hLightingOriginRelative, EHANDLE, "CBaseAnimating", "m_hLightingOriginRelative");
	NETVAR(m_flCycle, float, "CBaseAnimating", "m_flCycle");
	NETVAR(m_fadeMinDist, float, "CBaseAnimating", "m_fadeMinDist");
	NETVAR(m_fadeMaxDist, float, "CBaseAnimating", "m_fadeMaxDist");
	NETVAR(m_flFadeScale, float, "CBaseAnimating", "m_flFadeScale");

	int GetHitboxGroup(int nHitbox)
	{
		auto pModel = GetModel();

		if (!pModel)
			return -1;

		auto pHDR = I::ModelInfoClient->GetStudiomodel(pModel);

		if (!pHDR)
			return -1;

		auto pSet = pHDR->pHitboxSet(m_nHitboxSet());

		if (!pSet)
			return -1;

		auto pBox = pSet->pHitbox(nHitbox);

		if (!pBox)
			return -1;

		return pBox->group;
	}

	int GetNumOfHitboxes()
	{
		auto pModel = GetModel();

		if (!pModel)
			return 0;

		auto pHDR = I::ModelInfoClient->GetStudiomodel(pModel);

		if (!pHDR)
			return 0;

		auto pSet = pHDR->pHitboxSet(m_nHitboxSet());

		if (!pSet)
			return 0;

		return pSet->numhitboxes;
	}

	Vec3 GetHitboxPos(int nHitbox)
	{
		auto pModel = GetModel();

		if (!pModel)
			return {};

		auto pHDR = I::ModelInfoClient->GetStudiomodel(pModel);

		if (!pHDR)
			return {};

		auto pSet = pHDR->pHitboxSet(m_nHitboxSet());

		if (!pSet)
			return {};

		auto pBox = pSet->pHitbox(nHitbox);

		if (!pBox)
			return {};

		matrix3x4_t BoneMatrix[128] = {};

		if (!SetupBones(BoneMatrix, 128, BONE_USED_BY_ANYTHING, I::GlobalVars->curtime))
			return {};

		Vec3 vOut = {};
		Math::VectorTransform((pBox->bbmin + pBox->bbmax) * 0.5f, BoneMatrix[pBox->bone], vOut);
		return vOut;
	}

	void GetHitboxInfo(int nHitbox, Vec3 *pCenter, Vec3 *pMins, Vec3 *pMaxs, matrix3x4_t *pMatrix)
	{
		auto pModel = GetModel();

		if (!pModel)
			return;

		auto pHDR = I::ModelInfoClient->GetStudiomodel(pModel);

		if (!pHDR)
			return;

		auto pSet = pHDR->pHitboxSet(m_nHitboxSet());

		if (!pSet)
			return;

		auto pBox = pSet->pHitbox(nHitbox);

		if (!pBox)
			return;

		matrix3x4_t BoneMatrix[128] = {};

		if (!SetupBones(BoneMatrix, 128, BONE_USED_BY_ANYTHING, I::GlobalVars->curtime))
			return;

		if (pMins)
			*pMins = pBox->bbmin;

		if (pMaxs)
			*pMaxs = pBox->bbmax;

		if (pCenter)
			Math::VectorTransform((pBox->bbmin + pBox->bbmax) * 0.5f, BoneMatrix[pBox->bone], *pCenter);

		if (pMatrix)
			memcpy(*pMatrix, BoneMatrix[pBox->bone], sizeof(matrix3x4_t));
	}

	std::array<float, 24> &m_flPoseParameter() {
		static int nOffset = NetVars::GetNetVar("CBaseAnimating", "m_flPoseParameter");
		return *reinterpret_cast<std::array<float, 24>*>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	CUtlVector<matrix3x4_t> *GetCachedBoneData() {
		return reinterpret_cast<CUtlVector<matrix3x4_t> *>(reinterpret_cast<DWORD>(this) + 0x848); //0x844?
	}

	CStudioHdr *GetModelPtr() {
		static int nOffset = NetVars::GetNetVar("CBaseAnimating", "m_nMuzzleFlashParity") + 12;
		return *reinterpret_cast<CStudioHdr **>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	float FrameAdvance(float flInterval) {
		return reinterpret_cast<float(__thiscall *)(void *, float)>(Signatures::C_BaseAnimating_FrameAdvance.Get())(this, flInterval);
	}

	void GetBonePosition(int iBone, Vector &origin, QAngle &angles) {
		reinterpret_cast<void(__thiscall *)(void *, int, Vector &, QAngle &)>(Signatures::C_BaseAnimating_GetBonePosition.Get())(this, iBone, origin, angles);
	}
};

class C_BaseAnimatingOverlay : public C_BaseAnimating
{
public:

};

class C_CurrencyPack : public C_BaseAnimating
{
public:
	NETVAR(m_bDistributed, bool, "CCurrencyPack", "m_bDistributed");
};