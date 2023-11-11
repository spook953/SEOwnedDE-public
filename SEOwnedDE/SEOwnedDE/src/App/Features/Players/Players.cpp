#include "Players.h"

struct Player
{
	hash::hash_t m_steam_id{};
	players::PlayerInfo m_info{};
};

std::vector<Player> players_vec{};
std::string log_path{};

void players::parse()
{
	if (log_path.empty())
	{
		log_path = std::filesystem::current_path().string() + "\\SEOwnedDE\\";

		if (!std::filesystem::exists(log_path))
		{
			std::filesystem::create_directories(log_path);
		}

		log_path += "players.json";

		if (!std::filesystem::exists(log_path))
		{
			std::ofstream file(log_path, std::ios::app);

			if (!file.is_open())
			{
				return;
			}

			file.close();
		}
	}

	if (!players_vec.empty())
	{
		return;
	}

	std::ifstream file(log_path);

	if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof())
	{
		return;
	}

	nlohmann::json j{};

	file >> j;

	for (const auto &item : j.items())
	{
		Player p
		{ 
			HASH_RT(item.key().c_str()),
			{
				j[item.key()]["ignored"].get<bool>(),
				j[item.key()]["cheater"].get<bool>(),
				j[item.key()]["retardlegit"].get<bool>()
			} 
		};

		players_vec.push_back(p);
	}
}

void players::mark(int entindex, const PlayerInfo &info)
{
	if (entindex == I::EngineClient->GetLocalPlayer())
	{
		return;
	}

	player_info_t player_info{};

	if (!I::EngineClient->GetPlayerInfo(entindex, &player_info) || player_info.fakeplayer)
	{
		return;
	}

	Player *ptr{};

	auto steam_id{ HASH_RT(std::string_view(player_info.guid).data()) };

	for (auto &pl : players_vec)
	{
		if (pl.m_steam_id != steam_id)
		{
			continue;
		}

		ptr = &pl;

		break;
	}

	if (!ptr)
	{
		players_vec.push_back({ steam_id, info });

		ptr = &players_vec.back();
	}

	ptr->m_info = info;

	nlohmann::json j{};

	std::ifstream read_file(log_path);

	if (read_file.is_open() && read_file.peek() != std::ifstream::traits_type::eof())
	{
		read_file >> j;
	}

	read_file.close();

	std::ofstream file(log_path);

	if (!file.is_open())
	{
		return;
	}

	auto key{ std::string(player_info.guid) };

	j[key]["ignored"] = ptr->m_info.m_ignored;
	j[key]["cheater"] = ptr->m_info.m_cheater;
	j[key]["retardlegit"] = ptr->m_info.m_retard_legit;

	if (!ptr->m_info.m_ignored && !ptr->m_info.m_cheater && !ptr->m_info.m_retard_legit)
	{
		j.erase(std::string(player_info.guid));
	}

	file << std::setw(4) << j;

	file.close();
}

bool players::getInfo(int entindex, PlayerInfo &out)
{
	if (entindex == I::EngineClient->GetLocalPlayer())
	{
		return false;
	}

	player_info_t player_info{};

	if (!I::EngineClient->GetPlayerInfo(entindex, &player_info) || player_info.fakeplayer)
	{
		return false;
	}

	auto steam_id{ HASH_RT(std::string_view(player_info.guid).data()) };

	for (const auto &pl : players_vec)
	{
		if (pl.m_steam_id != steam_id)
		{
			continue;
		}

		out = pl.m_info;

		return true;
	}

	return false;
}

bool players::getInfoGUID(const std::string &guid, PlayerInfo &out)
{
	auto steam_id{ HASH_RT(guid.c_str()) };

	for (const auto &pl : players_vec)
	{
		if (pl.m_steam_id != steam_id)
		{
			continue;
		}

		out = pl.m_info;

		return true;
	}

	return false;
}