#pragma once

#include <vector>
#include <typeinfo>
#include <fstream>
#include <iomanip>

#include "json.hpp"

namespace Config
{
	struct ConfigVarInitializer
	{
		const char *m_name{ nullptr };
		void *m_ptr{ nullptr };
		size_t m_type_hash{ 0 };
		bool m_no_save{ false };
	};

	inline std::vector<ConfigVarInitializer> vars{};

	static void Save(const char *path)
	{
		std::ofstream output_file(path);

		if (!output_file.is_open())
		{
			return;
		}

		nlohmann::json j{};

		for (const auto &var : vars)
		{
			if (var.m_no_save)
			{
				continue;
			}

			if (var.m_type_hash == typeid(bool).hash_code())
			{
				j[var.m_name] = *static_cast<bool *>(var.m_ptr);
			}

			if (var.m_type_hash == typeid(int).hash_code())
			{
				j[var.m_name] = *static_cast<int *>(var.m_ptr);
			}

			if (var.m_type_hash == typeid(float).hash_code())
			{
				j[var.m_name] = *static_cast<float *>(var.m_ptr);
			}

			if (var.m_type_hash == typeid(Color_t).hash_code())
			{
				auto clr{ *static_cast<Color_t *>(var.m_ptr) };

				j[var.m_name] = { clr.r, clr.g, clr.b, clr.a };
			}

			if (var.m_type_hash == typeid(std::string).hash_code())
			{
				j[var.m_name] = *static_cast<std::string *>(var.m_ptr);
			}
		}

		output_file << std::setw(4) << j;

		output_file.close();
	}

	static void Load(const char *path)
	{
		std::ifstream input_file(path);

		if (!input_file.is_open())
		{
			return;
		}

		nlohmann::json j{};

		input_file >> j;

		for (const auto &var : vars)
		{
			if (var.m_no_save)
			{
				continue;
			}

			if (j.find(var.m_name) == j.end())
			{
				continue;
			}

			if (var.m_type_hash == typeid(bool).hash_code())
			{
				*static_cast<bool *>(var.m_ptr) = j[var.m_name];
			}

			if (var.m_type_hash == typeid(int).hash_code())
			{
				*static_cast<int *>(var.m_ptr) = j[var.m_name];
			}

			if (var.m_type_hash == typeid(float).hash_code())
			{
				*static_cast<float *>(var.m_ptr) = j[var.m_name];
			}

			if (var.m_type_hash == typeid(Color_t).hash_code())
			{
				Color_t clr{ j[var.m_name][0], j[var.m_name][1], j[var.m_name][2], j[var.m_name][3] };

				*static_cast<Color_t *>(var.m_ptr) = clr;
			}

			if (var.m_type_hash == typeid(std::string).hash_code())
			{
				*static_cast<std::string *>(var.m_ptr) = j[var.m_name];
			}
		}

		input_file.close();
	}
}

#define CFGVAR(var, val) inline auto var{ val }; \
namespace configvar_initializers\
{\
	inline auto var##_initializer = []()\
	{\
		Config::vars.push_back(Config::ConfigVarInitializer{#var, &var, typeid(var).hash_code(), false });\
		return true;\
	}();\
}

#define CFGVAR_NOSAVE(var, val) inline auto var{ val }; \
namespace configvar_initializers\
{\
	inline auto var##_initializer = []()\
	{\
		Config::vars.push_back(Config::ConfigVarInitializer{#var, &var, typeid(var).hash_code(), true });\
		return true;\
	}();\
}