#include "c_tf_player.h"

#include "../../App/Features/Players/Players.h"

bool C_TFPlayer::IsPlayerOnSteamFriendsList()
{
	auto result{ reinterpret_cast<bool(__thiscall *)(void *, void *)>(Signatures::C_TFPlayer_IsPlayerOnSteamFriendsList.Get())(this, this) };

	if (!result)
	{
		players::PlayerInfo info{};

		return players::getInfo(entindex(), info) && info.m_ignored;
	}

	return result;
}