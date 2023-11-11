#pragma once

#include "../../../SDK/SDK.h"

class CEnginePrediction
{
private:
	CMoveData m_MoveData = { };
	float m_fOldCurrentTime = 0.0f;
	float m_fOldFrameTime = 0.0f;
	int m_nOldTickCount = 0;

private:
	int GetTickbase(CUserCmd *pCmd, C_TFPlayer *pLocal);

public:
	void Start(CUserCmd *pCmd);
	void End();
	int flags{};
};

MAKE_SINGLETON_SCOPED(CEnginePrediction, EnginePrediction, F)