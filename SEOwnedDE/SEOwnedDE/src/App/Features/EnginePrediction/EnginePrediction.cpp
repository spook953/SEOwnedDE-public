#include "EnginePrediction.h"

#include "../CFG.h"

int CEnginePrediction::GetTickbase(CUserCmd *pCmd, C_TFPlayer *pLocal)
{
	static int nTick = 0;
	static CUserCmd *pLastCmd = nullptr;

	if (pCmd)
	{
		if (!pLastCmd || pLastCmd->hasbeenpredicted)
			nTick = pLocal->m_nTickBase();

		else nTick++;

		pLastCmd = pCmd;
	}

	return nTick;
}

void CEnginePrediction::Start(CUserCmd *pCmd)
{
	if (!I::MoveHelper)
		return;

	if (auto pLocal = H::Entities->GetLocal())
	{
		flags = pLocal->m_fFlags();

		pLocal->SetCurrentCommand(pCmd);

		static const int max = std::numeric_limits<int>::max();
		*I::RandomSeed = MD5_PseudoRandom(pCmd->command_number) & max;

		m_fOldCurrentTime = I::GlobalVars->curtime;
		m_fOldFrameTime = I::GlobalVars->frametime;
		m_nOldTickCount = I::GlobalVars->tickcount;

		const int nOldTickBase = pLocal->m_nTickBase();
		const bool bOldIsFirstPrediction = I::Prediction->m_bFirstTimePredicted;
		const bool bOldInPrediction = I::Prediction->m_bInPrediction;

		const int nServerTicks = GetTickbase(pCmd, pLocal);

		I::GlobalVars->curtime = TICKS_TO_TIME(nServerTicks);
		I::GlobalVars->frametime = (I::Prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL);
		I::GlobalVars->tickcount = nServerTicks;

		I::Prediction->m_bFirstTimePredicted = false;
		I::Prediction->m_bInPrediction = true;

		I::Prediction->SetLocalViewAngles(pCmd->viewangles);

		I::Prediction->SetupMove(pLocal, pCmd, I::MoveHelper, &m_MoveData);
		I::GameMovement->ProcessMovement(pLocal, &m_MoveData);
		I::Prediction->FinishMove(pLocal, pCmd, &m_MoveData);

		pLocal->m_nTickBase() = nOldTickBase;

		I::Prediction->m_bInPrediction = bOldInPrediction;
		I::Prediction->m_bFirstTimePredicted = bOldIsFirstPrediction;

		if (H::Input->IsDown(CFG::Misc_Edge_Jump_Key))
		{
			if ((flags & FL_ONGROUND) && !(pLocal->m_fFlags() & FL_ONGROUND))
			{
				pCmd->buttons |= IN_JUMP;
			}
		}
	}
}

void CEnginePrediction::End()
{
	if (auto pLocal = H::Entities->GetLocal())
	{
		I::GlobalVars->curtime = m_fOldCurrentTime;
		I::GlobalVars->frametime = m_fOldFrameTime;
		I::GlobalVars->tickcount = m_nOldTickCount;

		pLocal->SetCurrentCommand(nullptr);

		*I::RandomSeed = -1;
	}
}