#include "seed_pred.hpp"

#include "../CFG.h"
#include "../AimUtils/AimUtils.h"

bool synced{ false };
float server_time{ 0.0f };
float prev_server_time{ 0.0f };
float ask_time{ 0.0f };
float guess_time{ 0.0f };
float sync_offset{ 0.0f };
bool waiting_for_pp{ false };
float guess_delta{ 0.0f };
float response_time{ 0.0f };

void seed_pred::askForPlayerPerf()
{
	if (!CFG::Exploits_SeedPred_Active)
	{
		reset();

		return;
	}

	C_TFWeaponBase *weapon{ H::Entities->GetWeapon() };

	if (!weapon || !(weapon->GetDamageType() & DMG_BULLET) || F::AimUtils->GetWeaponType(weapon) != EWeaponType::HITSCAN || weapon->GetWeaponSpread() <= 0.0f)
	{
		reset();

		return;
	}

	if (C_TFPlayer *local{ H::Entities->GetLocal() })
	{
		if (local->deadflag())
		{
			reset();

			return;
		}
	}

	if (waiting_for_pp)
	{
		return;
	}

	I::ClientState->SendStringCmd("playerperf");

	ask_time = static_cast<float>(Plat_FloatTime());

	waiting_for_pp = true;
}

bool seed_pred::parsePlayerPerf(bf_read &msg_data)
{
	if (!CFG::Exploits_SeedPred_Active)
	{
		return false;
	}

	char raw_msg[256]{};

	msg_data.ReadString(raw_msg, sizeof(raw_msg), true);
	msg_data.Seek(0);

	std::string msg(raw_msg);

	msg.erase(msg.begin()); //STX

	std::smatch matches{};

	std::regex_match(msg, matches, std::regex("(\\d+.\\d+)\\s\\d+\\s\\d+\\s\\d+.\\d+\\s\\d+.\\d+\\svel\\s\\d+.\\d+"));

	if (matches.size() == 2)
	{
		waiting_for_pp = false;

		//credits to kgb for idea

		float new_server_time{ std::stof(matches[1].str()) };

		if (new_server_time > server_time)
		{
			prev_server_time = server_time;

			server_time = new_server_time;

			response_time = static_cast<float>(Plat_FloatTime() - ask_time);

			//if (!synced)
			{
				if (prev_server_time > 0.0f)
				{
					if (guess_time > 0.0f)
					{
						float delta{ server_time - guess_time };

						if (delta == 0.0f)
						{
							synced = true;

							sync_offset = guess_delta;
						}
					}

					guess_delta = server_time - prev_server_time;

					guess_time = server_time + (guess_delta);
				}
			}
		}
		
		return true;
	}

	else
	{
		return std::regex_match(msg, std::regex("\\d+.\\d+\\s\\d+\\s\\d+"));
	}

	return false;
}

int seed_pred::getSeed()
{
	float time{ (server_time + sync_offset + response_time) * 1000.0f };

	return *reinterpret_cast<int *>((char *)&time) & 255;
}

void seed_pred::reset()
{
	synced = false;
	server_time = 0.0f;
	prev_server_time = 0.0f;
	ask_time = 0.0f;
	guess_time = 0.0f;
	sync_offset = 0.0f;
	waiting_for_pp = false;
	guess_delta = 0.0f;
	response_time = 0.0f;
}

void seed_pred::adjustAngles(CUserCmd *cmd)
{
	if (!CFG::Exploits_SeedPred_Active || !synced || !cmd || !G::bFiring)
	{
		return;
	}

	auto local{ H::Entities->GetLocal() };

	if (!local)
	{
		return;
	}

	auto weapon{ H::Entities->GetWeapon() };

	if (!weapon || !(weapon->GetDamageType() & DMG_BULLET))
	{
		return;
	}

	auto spread{ weapon->GetWeaponSpread() };

	if (spread <= 0.0f)
	{
		return;
	}

	auto bullets_per_shot{ weapon->GetWeaponInfo()->GetWeaponData(TF_WEAPON_PRIMARY_MODE).m_nBulletsPerShot };

	bullets_per_shot = static_cast<int>(SDKUtils::AttribHookValue(static_cast<float>(bullets_per_shot), "mult_bullets_per_shot", weapon));

	//credits to cathook for average spread stuff

	std::vector<Vec3> bullet_corrections{};

	Vec3 average_spread{};

	auto seed{ getSeed() };

	for (auto bullet{ 0 }; bullet < bullets_per_shot; bullet++)
	{
		SDKUtils::RandomSeed(seed++);

		auto fire_perfect{ false };

		if (bullet == 0)
		{
			auto time_since_last_shot{ (local->m_nTickBase() * TICK_INTERVAL) - weapon->m_flLastFireTime() };

			if (bullets_per_shot > 1 && time_since_last_shot > 0.25f)
			{
				fire_perfect = true;
			}

			else if (bullets_per_shot == 1 && time_since_last_shot > 1.25f)
			{
				fire_perfect = true;
			}
		}

		if (fire_perfect)
		{
			return;
		}

		auto x{ SDKUtils::RandomFloat(-0.5f, 0.5f) + SDKUtils::RandomFloat(-0.5f, 0.5f) };
		auto y{ SDKUtils::RandomFloat(-0.5f, 0.5f) + SDKUtils::RandomFloat(-0.5f, 0.5f) };

		Vec3 forward{}, right{}, up{};

		Math::AngleVectors(cmd->viewangles, &forward, &right, &up);

		Vector fixed_spread{ forward + (right * x * spread) + (up * y * spread) };

		fixed_spread.NormalizeInPlace();

		average_spread += fixed_spread;

		bullet_corrections.push_back(fixed_spread);
	}

	average_spread /= static_cast<float>(bullets_per_shot);

	Vec3 fixed_spread{ FLT_MAX, FLT_MAX, FLT_MAX };

	for (const auto &spread : bullet_corrections)
	{
		if (spread.DistTo(average_spread) < fixed_spread.DistTo(average_spread))
		{
			fixed_spread = spread;
		}
	}

	Vec3 fixed_angles{};

	Math::VectorAngles(fixed_spread, fixed_angles);

	Vec3 correction{ cmd->viewangles - fixed_angles };

	cmd->viewangles += correction;

	Math::ClampAngles(cmd->viewangles);

	G::bSilentAngles = true;
}

void seed_pred::paint()
{
	if (!CFG::Exploits_SeedPred_Active || I::EngineVGui->IsGameUIVisible() || SDKUtils::BInEndOfMatch() || server_time <= 0.0f)
	{
		return;
	}

	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	if (CFG::Exploits_SeedPred_DrawIndicator)
	{
		auto calcMantissaStep = [](float val)
		{
			auto raw_val{ reinterpret_cast<int &>(val) };
			auto exponent{ (raw_val >> 23) & 0xFF };

			auto result{ powf(2.0f, static_cast<float>(exponent - (127 + 23))) * 1000.0f };

			static std::vector<float> mantissas{};

			if (mantissas.empty())
			{
				auto mantissa{ 1.0f };

				for (auto n{ 0 }; n < 16; n++)
				{
					mantissas.push_back(mantissa);

					mantissa *= 2.0f;
				}
			}

			auto closest = [](const std::vector<float> &vec, float value)
			{
				auto it{ std::lower_bound(vec.begin(), vec.end(), value) };

				if (it == vec.end())
				{
					return value;
				}

				return *it;
			};

			return closest(mantissas, result);
		};

		std::chrono::hh_mm_ss time{ std::chrono::seconds(static_cast<int>(server_time)) };

		int x{ 2 };
		int y{ 2 };

		H::Draw->String
		(
			H::Fonts->Get(EFonts::ESP_SMALL),
			x, y,
			{ 200, 200, 200, 255 }, POS_DEFAULT,
			std::format("{}h {}m {}s (step {:.0f})", time.hours().count(), time.minutes().count(), time.seconds().count(), calcMantissaStep(server_time)).c_str()
		);

		y += 10;

		H::Draw->String
		(
			H::Fonts->Get(EFonts::ESP_SMALL),
			x, y,
			!synced ? Color_t{ 250, 130, 49, 255 } : Color_t{ 32, 191, 107, 255 }, POS_DEFAULT,
			!synced ? "syncing.." : std::format("synced ({})", sync_offset).c_str()
		);
	}
}