#include "../../SDK/SDK.h"

#include "../Features/CFG.h"
#include "../Features/WorldModulation/WorldModulation.h"
#include "../Features/LagRecords/LagRecords.h"
#include "../Features/MiscVisuals/MiscVisuals.h"

MAKE_HOOK(
	IBaseClientDLL_FrameStageNotify, Memory::GetVFunc(I::BaseClientDLL, 35),
	void, __fastcall, void *ecx, void *edx, ClientFrameStage_t curStage)
{
	CALL_ORIGINAL(ecx, edx, curStage);

	switch (curStage)
	{
		case ClientFrameStage_t::FRAME_NET_UPDATE_START:
		{
			H::Entities->ClearCache();

			break;
		}

		case ClientFrameStage_t::FRAME_NET_UPDATE_END:
		{
			H::Entities->UpdateCache();

			if (auto pLocal = H::Entities->GetLocal())
			{
				for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ALL))
				{
					if (!pEntity || pEntity == pLocal)
						continue;

					auto pPlayer = pEntity->As<C_TFPlayer>();

					if (auto nDifference = std::clamp(TIME_TO_TICKS(pPlayer->m_flSimulationTime() - pPlayer->m_flOldSimulationTime()), 0, 22))
					{
						//deal with animations, local player is dealt with in RunCommand
						if (CFG::Misc_Accuracy_Improvements)
						{
							float flOldFrameTime = I::GlobalVars->frametime;

							I::GlobalVars->frametime = I::Prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL;

							for (int n = 0; n < nDifference; n++)
							{
								G::bUpdatingAnims = true;
								pPlayer->UpdateClientSideAnimation();
								G::bUpdatingAnims = false;
							}

							I::GlobalVars->frametime = flOldFrameTime;
						}

						//add the lag record
						if (CFG::Misc_SetupBones_Optimization)
						{
							if (!pPlayer->deadflag())
							{
								F::LagRecords->AddRecord(pPlayer);
							}
						}

						else
						{
							if (pPlayer->m_iTeamNum() != pLocal->m_iTeamNum() && !pPlayer->deadflag())
							{
								F::LagRecords->AddRecord(pPlayer);
							}
						}
					}
				}
			}

			F::LagRecords->UpdateRecords();

			if (G::mapVelFixRecords.size() > 64)
				G::mapVelFixRecords.clear();

			for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ALL))
			{
				if (!pEntity)
					continue;

				auto pPlayer = pEntity->As<C_TFPlayer>();

				if (pPlayer->deadflag())
					continue;

				G::mapVelFixRecords[pPlayer] = { pPlayer->m_vecOrigin(), pPlayer->m_fFlags(), pPlayer->m_flSimulationTime() };
			}

			break;
		}

		case ClientFrameStage_t::FRAME_RENDER_START:
		{
			H::Input->Update();

			F::WorldModulation->UpdateWorldModulation();
			F::MiscVisuals->ViewModelSway();
			F::MiscVisuals->DetailProps();

			//fake taunt stuff
			{
				static bool bWasEnabled = false;

				if (CFG::Misc_Fake_Taunt)
				{
					bWasEnabled = true;

					if (G::bStartedFakeTaunt)
					{
						if (auto pLocal = H::Entities->GetLocal())
						{
							if (auto pAnimState = pLocal->GetAnimState())
							{
								const auto &GS = pAnimState->m_aGestureSlots[GESTURE_SLOT_VCD];

								if (GS.m_pAnimLayer && (GS.m_pAnimLayer->m_flCycle >= 1.0f || GS.m_pAnimLayer->m_nSequence <= 0)) {
									G::bStartedFakeTaunt = false;
									pLocal->m_nForceTauntCam() = 0;
								}
							}
						}
					}
				}

				else
				{
					G::bStartedFakeTaunt = false;

					if (bWasEnabled)
					{
						bWasEnabled = false;

						if (auto pLocal = H::Entities->GetLocal())
							pLocal->m_nForceTauntCam() = 0;
					}
				}
			}

			break;
		}

		default: break;
	}
}