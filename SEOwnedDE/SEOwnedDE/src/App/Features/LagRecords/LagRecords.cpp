#include "LagRecords.h"

#include "../CFG.h"

void CLagRecords::EraseRecord(C_TFPlayer *pPlayer, int nRecord)
{
	auto &v = m_LagRecords[pPlayer];
	v.erase(v.begin() + nRecord);
}

void CLagRecords::EraseAllRecords(C_TFPlayer *pPlayer)
{
	if (m_LagRecords.find(pPlayer) != m_LagRecords.end())
		m_LagRecords.erase(pPlayer);
}

bool CLagRecords::IsSimulationTimeValid(float flCurSimTime, float flCmprSimTime)
{
	return flCurSimTime - flCmprSimTime < 0.2f;
}

void CLagRecords::AddRecord(C_TFPlayer *pPlayer)
{
	LagRecord_t Record = {};

	m_bSettingUpBones = true;

	auto setup_bones_optimization{ CFG::Misc_SetupBones_Optimization };

	if (setup_bones_optimization)
	{
		pPlayer->InvalidateBoneCache();
	}

	auto result = pPlayer->SetupBones(Record.m_BoneMatrix, 128, BONE_USED_BY_ANYTHING, I::GlobalVars->curtime);

	if (setup_bones_optimization)
	{
		for (auto n{ 0 }; n < 32; n++)
		{
			auto attach{ pPlayer->FirstMoveChild() };

			if (!attach)
			{
				break;
			}

			if (attach->ShouldDraw())
			{
				attach->InvalidateBoneCache();
				attach->SetupBones(nullptr, -1, BONE_USED_BY_ANYTHING, I::GlobalVars->curtime);
			}

			attach = attach->NextMovePeer();
		}
	}

	m_bSettingUpBones = false;

	if (!result)
		return;

	Record.m_pPlayer = pPlayer;
	Record.m_flSimulationTime = pPlayer->m_flSimulationTime();
	Record.m_vAbsOrigin = pPlayer->GetAbsOrigin();
	Record.m_vVecOrigin = pPlayer->m_vecOrigin();
	Record.m_vAbsAngles = pPlayer->GetAbsAngles();
	Record.m_vEyeAngles = pPlayer->GetEyeAngles();
	Record.m_vVelocity = pPlayer->m_vecVelocity();
	Record.m_vCenter = pPlayer->GetCenter();
	Record.m_nFlags = pPlayer->m_fFlags();

	if (auto pAnimState = pPlayer->GetAnimState())
		Record.m_flFeetYaw = pAnimState->m_flCurrentFeetYaw;

	m_LagRecords[pPlayer].emplace_front(Record);
}

const LagRecord_t *CLagRecords::GetRecord(C_TFPlayer *pPlayer, int nRecord, bool bSafe)
{
	if (!bSafe)
	{
		if (m_LagRecords.find(pPlayer) == m_LagRecords.end())
			return nullptr;

		if (nRecord < 0 || nRecord > static_cast<int>(m_LagRecords[pPlayer].size() - 1))
			return nullptr;
	}

	return &m_LagRecords[pPlayer][nRecord];
}

bool CLagRecords::HasRecords(C_TFPlayer *pPlayer, int *pTotalRecords)
{
	if (m_LagRecords.find(pPlayer) != m_LagRecords.end())
	{
		size_t nSize = m_LagRecords[pPlayer].size();

		if (nSize <= 0)
			return false;

		if (pTotalRecords)
			*pTotalRecords = static_cast<int>(nSize - 1);

		return true;
	}

	return false;
}

void CLagRecords::UpdateRecords()
{
	auto pLocal = H::Entities->GetLocal();

	if (!pLocal || pLocal->deadflag() || pLocal->InCond(TF_COND_HALLOWEEN_GHOST_MODE) || pLocal->InCond(TF_COND_HALLOWEEN_KART))
	{
		for (const auto &Player : m_LagRecords)
		{
			EraseAllRecords(Player.first);
		}

		return;
	}

	for (auto pEntity : H::Entities->GetGroup(CFG::Misc_SetupBones_Optimization ? EEntGroup::PLAYERS_ALL : EEntGroup::PLAYERS_ENEMIES))
	{
		if (!pEntity || pEntity == pLocal)
		{
			continue;
		}

		auto pPlayer = pEntity->As<C_TFPlayer>();

		if (pPlayer->deadflag())
		{
			EraseAllRecords(pPlayer);

			continue;
		}
	}
	
	for (const auto &Record : m_LagRecords)
	{
		for (size_t n = 0; n < Record.second.size(); n++)
		{
			auto &CurRecord = Record.second[n];

			if (!CurRecord.m_pPlayer || !IsSimulationTimeValid(CurRecord.m_pPlayer->m_flSimulationTime(), CurRecord.m_flSimulationTime))
			{
				EraseRecord(CurRecord.m_pPlayer, n);
			}
		}
	}
}

bool CLagRecords::DiffersFromCurrent(const LagRecord_t *pRecord)
{
	auto pPlayer = pRecord->m_pPlayer;

	if (!pPlayer)
		return false;

	if (static_cast<int>((pPlayer->m_vecOrigin() - pRecord->m_vAbsOrigin).Length()) != 0)
		return true;

	if (static_cast<int>((pPlayer->GetEyeAngles() - pRecord->m_vEyeAngles).Length()) != 0)
		return true;

	if (pPlayer->m_fFlags() != pRecord->m_nFlags)
		return true;

	if (auto pAnimState = pPlayer->GetAnimState())
	{
		if (fabsf(pAnimState->m_flCurrentFeetYaw - pRecord->m_flFeetYaw) > 0.0f)
			return true;
	}

	return false;
}

void CLagRecordMatrixHelper::Set(const LagRecord_t *pRecord)
{
	if (!pRecord)
		return;

	auto pPlayer = pRecord->m_pPlayer;

	if (!pPlayer || pPlayer->deadflag())
		return;

	auto pCachedBoneData = pPlayer->GetCachedBoneData();

	if (!pCachedBoneData)
		return;

	m_pPlayer = pPlayer;
	m_vAbsOrigin = pPlayer->GetAbsOrigin();
	m_vAbsAngles = pPlayer->GetAbsAngles();
	memcpy(m_BoneMatrix, pCachedBoneData->Base(), sizeof(matrix3x4_t) * pCachedBoneData->Count());

	memcpy(pCachedBoneData->Base(), pRecord->m_BoneMatrix, sizeof(matrix3x4_t) * pCachedBoneData->Count());

	pPlayer->SetAbsOrigin(pRecord->m_vAbsOrigin);
	pPlayer->SetAbsAngles(pRecord->m_vAbsAngles);

	m_bSuccessfullyStored = true;
}

void CLagRecordMatrixHelper::Restore()
{
	if (!m_bSuccessfullyStored || !m_pPlayer)
		return;

	auto pCachedBoneData = m_pPlayer->GetCachedBoneData();

	if (!pCachedBoneData)
		return;

	m_pPlayer->SetAbsOrigin(m_vAbsOrigin);
	m_pPlayer->SetAbsAngles(m_vAbsAngles);
	memcpy(pCachedBoneData->Base(), m_BoneMatrix, sizeof(matrix3x4_t) * pCachedBoneData->Count());

	m_pPlayer = nullptr;
	m_vAbsOrigin = {};
	m_vAbsAngles = {};
	std::memset(m_BoneMatrix, 0, sizeof(matrix3x4_t) * 128);
	m_bSuccessfullyStored = false;
}