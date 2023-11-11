#pragma once

#include "../../../SDK/SDK.h"

class CMovementSimulation
{
private:
	C_TFPlayer *m_pPlayer = nullptr;
	CMoveData m_MoveData = {};
	bool m_bRunning = false;
	float m_flYawTurnRate = 0.0f;

private:
	void SetupMoveData(C_TFPlayer *pPlayer, CMoveData *pMoveData);

private:
	bool m_bOldInPrediction = false;
	bool m_bOldFirstTimePredicted = false;
	float m_flOldFrametime = 0.0f;

public:
	bool Initialize(C_TFPlayer *pPlayer);
	void Restore();
	void RunTick(float flTimeToTarget = 0.0f);

public:
	inline const Vec3 &GetOrigin() { return m_MoveData.m_vecAbsOrigin; }
	inline bool IsRunning() { return m_bRunning; }
};

MAKE_SINGLETON_SCOPED(CMovementSimulation, MovementSimulation, F);