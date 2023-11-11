#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CPrediction_RunCommand, Memory::GetVFunc(I::Prediction, 17),
	void, __fastcall, void *ecx, void *edx, C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper)
{
	if (Shifting::bRecharging)
	{
		if (auto pLocal = H::Entities->GetLocal())
		{
			if (player == pLocal)
				return;
		}
	}

	CALL_ORIGINAL(ecx, edx, player, ucmd, moveHelper);

	I::MoveHelper = moveHelper;

	if (auto pLocal = H::Entities->GetLocal())
	{
		//credits: KGB
		if (CFG::Misc_Accuracy_Improvements && !pLocal->InCond(TF_COND_HALLOWEEN_KART) && !Shifting::bRecharging)
		{
			if (!ucmd->hasbeenpredicted && player == pLocal)
			{
				if (auto pAnimState = pLocal->GetAnimState())
				{
					float flOldFrameTime = I::GlobalVars->frametime;
					I::GlobalVars->frametime = I::Prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL;
					pAnimState->Update(G::bStartedFakeTaunt ? G::flFakeTauntStartYaw : ucmd->viewangles.y, ucmd->viewangles.x);
					pLocal->FrameAdvance(I::GlobalVars->frametime);
					I::GlobalVars->frametime = flOldFrameTime;
				}
			}
		}
	}
}