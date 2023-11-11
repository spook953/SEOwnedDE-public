#include "../../SDK/SDK.h"

#include "../Features/CFG.h"
#include "../Features/AutoVaccinator/AutoVaccinator.h"
#include "../Features/Players/Players.h"

void onVoteCast(IGameEvent *event)
{
	if (!CFG::Visuals_Chat_Teammate_Votes && !CFG::Visuals_Chat_Enemy_Votes)
		return;

	if (event->GetInt("team") == -1)
		return;

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal)
		return;

	if (!CFG::Visuals_Chat_Teammate_Votes && pLocal->m_iTeamNum() == event->GetInt("team"))
		return;

	if (!CFG::Visuals_Chat_Enemy_Votes && pLocal->m_iTeamNum() != event->GetInt("team"))
		return;

	player_info_t pi{};

	if (!I::EngineClient->GetPlayerInfo(event->GetInt("entityid"), &pi))
		return;

	if (event->GetInt("vote_option") == 0)
	{
		I::ClientModeShared->PrintToChat(std::format("\x1{} voted \x8{}YES", pi.name, Color_t{ 46, 204, 113, 255 }.toHexStr()).c_str());
	}

	if (event->GetInt("vote_option") == 1)
	{
		I::ClientModeShared->PrintToChat(std::format("\x1{} voted \x8{}NO", pi.name, Color_t{ 231, 76, 60, 255 }.toHexStr()).c_str());
	}
}

MAKE_HOOK(
	CGameEventManager_FireEventIntern, Signatures::CGameEventManager_FireEventIntern.Get(),
	bool, __fastcall, void *ecx, void *edx, IGameEvent *event, bool bServerOnly, bool bClientOnly)
{
	if (event)
	{
		static const auto vote_cast{ HASH_CT("vote_cast") };
		static const auto player_hurt{ HASH_CT("player_hurt") };
		static const auto revive_player_notify{ HASH_CT("revive_player_notify") };
		static const auto player_connect_client{ HASH_CT("player_connect_client") };

		if (HASH_RT(event->GetName()) == vote_cast)
		{
			onVoteCast(event);
		}

		if (HASH_RT(event->GetName()) == player_hurt)
		{
			auto_vaccinator::processPlayerHurt(event);
		}

		if (HASH_RT(event->GetName()) == player_connect_client && bClientOnly && CFG::Visuals_Chat_Player_List_Info)
		{
			players::PlayerInfo pi{};

			if (players::getInfoGUID(event->GetString("networkid"), pi))
			{
				const char *const name{ event->GetString("name") };

				if (pi.m_ignored)
				{
					I::ClientModeShared->PrintToChat(std::format("\x1{} is marked as \x8{}[Ignored]", name, CFG::Color_Friend.toHexStr()).c_str());
				}

				if (pi.m_cheater)
				{
					I::ClientModeShared->PrintToChat(std::format("\x1{} is marked as \x8{}[Cheater]", name, CFG::Color_Cheater.toHexStr()).c_str());
				}

				if (pi.m_retard_legit)
				{
					I::ClientModeShared->PrintToChat(std::format("\x1{} is marked as \x8{}[Retard Legit]", name, CFG::Color_RetardLegit.toHexStr()).c_str());
				}
			}
		}

		if (auto local{ H::Entities->GetLocal() })
		{
			if (CFG::Misc_MVM_Instant_Revive && HASH_RT(event->GetName()) == revive_player_notify)
			{
				if (event->GetInt("entindex") == local->entindex())
				{
					auto *kv{ new KeyValues("MVM_Revive_Response") };

					kv->SetInt("accepted", 1);

					I::EngineClient->ServerCmdKeyValues(kv);
				}
			}
		}
	}

	return CALL_ORIGINAL(ecx, edx, event, bServerOnly, bClientOnly);
}