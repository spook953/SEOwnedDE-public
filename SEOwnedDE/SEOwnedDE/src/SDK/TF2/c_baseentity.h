#pragma once
#include "icliententity.h"
#include "basehandle.h"
#include "utlvector.h"
#include "const.h"
#include "tf_shareddefs.h"
#include "icliententitylist.h"
#include "Signatures.h"
#include "NetVars/NetVars.h"

enum CollideType_t
{
	ENTITY_SHOULD_NOT_COLLIDE = 0,
	ENTITY_SHOULD_COLLIDE,
	ENTITY_SHOULD_RESPOND
};

typedef CHandle<C_BaseEntity> EHANDLE;

#define MULTIPLAYER_BACKUP 90

class IInterpolatedVar;

class VarMapEntry_t
{
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate;
	void *data;
	IInterpolatedVar *watcher;
};

struct VarMapping_t
{
	CUtlVector<VarMapEntry_t> m_Entries;
	int m_nInterpolatedEntries;
	float m_lastInterpolationTime;
};

class C_BaseEntity : public IClientEntity
{
public:
	NETVAR(m_flAnimTime, float, "CBaseEntity", "m_flAnimTime");
	NETVAR(m_flSimulationTime, float, "CBaseEntity", "m_flSimulationTime");
	NETVAR(m_ubInterpolationFrame, int, "CBaseEntity", "m_ubInterpolationFrame");
	NETVAR(m_vecOrigin, Vec3, "CBaseEntity", "m_vecOrigin");
	NETVAR(m_angRotation, Vec3, "CBaseEntity", "m_angRotation");
	NETVAR(m_nModelIndex, int, "CBaseEntity", "m_nModelIndex");
	NETVAR(m_fEffects, int, "CBaseEntity", "m_fEffects");
	NETVAR(m_nRenderMode, int, "CBaseEntity", "m_nRenderMode");
	NETVAR(m_nRenderFX, int, "CBaseEntity", "m_nRenderFX");
	NETVAR(m_clrRender, Color_t, "CBaseEntity", "m_clrRender");
	NETVAR(m_iTeamNum, int, "CBaseEntity", "m_iTeamNum");
	NETVAR(m_CollisionGroup, int, "CBaseEntity", "m_CollisionGroup");
	NETVAR(m_flElasticity, float, "CBaseEntity", "m_flElasticity");
	NETVAR(m_flShadowCastDistance, float, "CBaseEntity", "m_flShadowCastDistance");
	NETVAR(m_hOwnerEntity, EHANDLE, "CBaseEntity", "m_hOwnerEntity");
	NETVAR(m_hEffectEntity, EHANDLE, "CBaseEntity", "m_hEffectEntity");
	NETVAR(moveparent, int, "CBaseEntity", "moveparent");
	NETVAR(m_iParentAttachment, int, "CBaseEntity", "m_iParentAttachment");
	NETVAR(m_Collision, void *, "CBaseEntity", "m_Collision");
	NETVAR(m_vecMinsPreScaled, Vec3, "CBaseEntity", "m_vecMinsPreScaled");
	NETVAR(m_vecMaxsPreScaled, Vec3, "CBaseEntity", "m_vecMaxsPreScaled");
	NETVAR(m_vecMins, Vec3, "CBaseEntity", "m_vecMins");
	NETVAR(m_vecMaxs, Vec3, "CBaseEntity", "m_vecMaxs");
	NETVAR(m_nSolidType, int, "CBaseEntity", "m_nSolidType");
	NETVAR(m_usSolidFlags, int, "CBaseEntity", "m_usSolidFlags");
	NETVAR(m_nSurroundType, int, "CBaseEntity", "m_nSurroundType");
	NETVAR(m_triggerBloat, int, "CBaseEntity", "m_triggerBloat");
	NETVAR(m_bUniformTriggerBloat, bool, "CBaseEntity", "m_bUniformTriggerBloat");
	NETVAR(m_vecSpecifiedSurroundingMinsPreScaled, Vec3, "CBaseEntity", "m_vecSpecifiedSurroundingMinsPreScaled");
	NETVAR(m_vecSpecifiedSurroundingMaxsPreScaled, Vec3, "CBaseEntity", "m_vecSpecifiedSurroundingMaxsPreScaled");
	NETVAR(m_vecSpecifiedSurroundingMins, Vec3, "CBaseEntity", "m_vecSpecifiedSurroundingMins");
	NETVAR(m_vecSpecifiedSurroundingMaxs, Vec3, "CBaseEntity", "m_vecSpecifiedSurroundingMaxs");
	NETVAR(m_iTextureFrameIndex, int, "CBaseEntity", "m_iTextureFrameIndex");
	NETVAR(m_PredictableID, int, "CBaseEntity", "m_PredictableID");
	NETVAR(m_bIsPlayerSimulated, bool, "CBaseEntity", "m_bIsPlayerSimulated");
	NETVAR(m_bSimulatedEveryTick, bool, "CBaseEntity", "m_bSimulatedEveryTick");
	NETVAR(m_bAnimatedEveryTick, bool, "CBaseEntity", "m_bAnimatedEveryTick");
	NETVAR(m_bAlternateSorting, bool, "CBaseEntity", "m_bAlternateSorting");
	NETVAR(m_nModelIndexOverrides, void *, "CBaseEntity", "m_nModelIndexOverrides");
	NETVAR(movetype, int, "CBaseEntity", "movetype");

	Vec3 GetCenter()
	{
		Vec3 vMin = {}, vMax = {};
		GetRenderBounds(vMin, vMax);
		return m_vecOrigin() + Vec3(0.0f, 0.0f, (vMin.z + vMax.z) / 2.0f);
	}

	Vec3 GetRenderCenter()
	{
		Vec3 vMin = {}, vMax = {};
		GetRenderBounds(vMin, vMax);
		return GetRenderOrigin() + Vec3(0.0f, 0.0f, (vMin.z + vMax.z) / 2.0f);
	}

	bool IsInValidTeam(int *pTeamNumOut = nullptr)
	{
		int nTeamNum = m_iTeamNum();

		switch (nTeamNum)
		{
			case TF_TEAM_RED:
			case TF_TEAM_BLUE:
			{
				if (pTeamNumOut)
					*pTeamNumOut = nTeamNum;

				return true;
			}

			default: return false;
		}
	}

	float &m_flOldSimulationTime() {
		static int nOffset = (NetVars::GetNetVar("CBaseEntity", "m_flSimulationTime") + 4);
		return *reinterpret_cast<float *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	C_BaseEntity *GetMoveParent()
	{
		static auto offset{ NetVars::GetNetVar("CBaseEntity", "moveparent") - 4 };

		auto m_pMoveParent{ reinterpret_cast<EHANDLE *>(reinterpret_cast<uintptr_t>(this) + offset) };

		if (!m_pMoveParent)
		{
			return nullptr;
		}

		return m_pMoveParent->Get();
	}

	C_BaseEntity *FirstMoveChild()
	{
		static auto offset{ NetVars::GetNetVar("CBaseEntity", "moveparent") - 12 };
		
		auto m_pMoveChild{ reinterpret_cast<EHANDLE *>(reinterpret_cast<uintptr_t>(this) + offset) };

		if (!m_pMoveChild)
		{
			return nullptr;
		}

		return m_pMoveChild->Get();
	}

	C_BaseEntity *NextMovePeer()
	{
		static auto offset{ NetVars::GetNetVar("CBaseEntity", "moveparent") - 8 };

		auto m_pMovePeer{ reinterpret_cast<EHANDLE *>(reinterpret_cast<uintptr_t>(this) + offset) };

		if (!m_pMovePeer)
		{
			return nullptr;
		}

		return m_pMovePeer->Get();
	}

	void UpdateVisibility() {
		reinterpret_cast<void(__thiscall *)(C_BaseEntity *)>(Memory::GetVFunc(this, 91))(this);
	}

	unsigned char &m_MoveType() {
		static int nOffset = 420;
		return *reinterpret_cast<unsigned char *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	unsigned char &m_MoveCollide() {
		static int nOffset = 421;
		return *reinterpret_cast<unsigned char *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	float &m_flGravity() {
		static int nOffset = 400;
		return *reinterpret_cast<float *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	unsigned char &m_nWaterLevel_C_BaseEntity() {
		static int nOffset = 424;
		return *reinterpret_cast<unsigned char *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	unsigned char &m_nWaterType() {
		static int nOffset = 425;
		return *reinterpret_cast<unsigned char *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	void SetAbsOrigin(const Vec3 &absOrigin) {
		reinterpret_cast<void(__thiscall *)(void *, const Vec3 &)>(Signatures::C_BaseEntity_SetAbsOrigin.Get())(this, absOrigin);
	}

	void SetAbsAngles(const Vec3 &absAngles) {
		reinterpret_cast<void(__thiscall *)(void *, const Vec3 &)>(Signatures::C_BaseEntity_SetAbsAngles.Get())(this, absAngles);
	}

	void SetAbsVelocity(const Vector &vecAbsVelocity) {
		reinterpret_cast<void(__thiscall *)(void *, const Vec3 &)>(Signatures::C_BaseEntity_SetAbsVelocity.Get())(this, vecAbsVelocity);
	}

	void EstimateAbsVelocity(Vector &vel) {
		reinterpret_cast<void(__thiscall *)(void *, Vector &)>(Signatures::C_BaseEntity_EstimateAbsVelocity.Get())(this, vel);
	}

	void CreateShadow() {
		reinterpret_cast<void(__thiscall *)(void *)>(Signatures::C_BaseEntity_CreateShadow.Get())(this);
	}

	void InvalidateBoneCache()
	{
		reinterpret_cast<void(__thiscall *)(void *)>(Signatures::C_BaseEntity_InvalidateBoneCache.Get())(this);
	}
};