#pragma once

#include "../../../SDK/SDK.h"

struct LagRecord_t
{
	C_TFPlayer *m_pPlayer = nullptr;
	matrix3x4_t m_BoneMatrix[128] = {};
	float m_flSimulationTime = -1.0f;
	Vec3 m_vAbsOrigin = {};
	Vec3 m_vVecOrigin = {};
	Vec3 m_vAbsAngles = {};
	Vec3 m_vEyeAngles = {};
	Vec3 m_vVelocity = {};
	Vec3 m_vCenter = {};
	int m_nFlags = 0;
	float m_flFeetYaw = 0.0f;
};

class CLagRecords
{
private:
	std::map<C_TFPlayer *, std::deque<LagRecord_t>> m_LagRecords = {};
	bool m_bSettingUpBones = false;

	void EraseRecord(C_TFPlayer *pPlayer, int nRecord);
	void EraseAllRecords(C_TFPlayer *pPlayer);

private:
	bool IsSimulationTimeValid(float flCurSimTime, float flCmprSimTime);

public:
	void AddRecord(C_TFPlayer *pPlayer);
	const LagRecord_t *GetRecord(C_TFPlayer *pPlayer, int nRecord, bool bSafe = false);
	bool HasRecords(C_TFPlayer *pPlayer, int *pTotalRecords = nullptr);
	void UpdateRecords();
	bool DiffersFromCurrent(const LagRecord_t *pRecord);
	inline bool IsSettingUpBones() { return m_bSettingUpBones; }
};

MAKE_SINGLETON_SCOPED(CLagRecords, LagRecords, F);

class CLagRecordMatrixHelper
{
private:
	C_TFPlayer *m_pPlayer = nullptr;
	Vec3 m_vAbsOrigin = {};
	Vec3 m_vAbsAngles = {};
	matrix3x4_t m_BoneMatrix[128] = {};

	bool m_bSuccessfullyStored = false;

public:
	void Set(const LagRecord_t *pRecord);
	void Restore();
};

MAKE_SINGLETON_SCOPED(CLagRecordMatrixHelper, LagRecordMatrixHelper, F);