#include "../../SDK/SDK.h"

#include "../Features/Players/Players.h"
#include "../Features/CFG.h"

MAKE_HOOK(
	IBaseClientDLL_LevelInitPostEntity, Memory::GetVFunc(I::BaseClientDLL, 6),
	void, __fastcall, void *ecx, void *edx)
{
	CALL_ORIGINAL(ecx, edx);

	H::Entities->UpdateModelIndexes();

	if (CFG::Visuals_Chat_Player_List_Info)
	{
		for (int n{}; n < I::EngineClient->GetMaxClients() + 1; n++)
		{
			players::PlayerInfo pi{};

			if (players::getInfo(n, pi))
			{
				player_info_t pi_game{};

				if (!I::EngineClient->GetPlayerInfo(n, &pi_game))
				{
					continue;
				}

				if (pi.m_ignored)
				{
					I::ClientModeShared->PrintToChat(std::format("\x1{} is marked as \x8{}[Ignored]", pi_game.name, CFG::Color_Friend.toHexStr()).c_str());
				}

				if (pi.m_cheater)
				{
					I::ClientModeShared->PrintToChat(std::format("\x1{} is marked as \x8{}[Cheater]", pi_game.name, CFG::Color_Cheater.toHexStr()).c_str());
				}

				if (pi.m_retard_legit)
				{
					I::ClientModeShared->PrintToChat(std::format("\x1{} is marked as \x8{}[Retard Legit]", pi_game.name, CFG::Color_RetardLegit.toHexStr()).c_str());
				}
			}
		}
	}
}