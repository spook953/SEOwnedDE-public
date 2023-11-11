#pragma once

#include "../../../SDK/SDK.h"

struct ProjInfo
{
	ProjectileType_t m_type{};

	Vec3 m_pos{};
	Vec3 m_ang{};

	float m_speed{};
	float m_gravity_mod{};
};

namespace proj_sim
{
	bool getInfo(C_TFPlayer *player, C_TFWeaponBase *weapon, const Vec3 &angles, ProjInfo &out);
	bool init(const ProjInfo &info);
	void runTick();
	Vec3 getOrigin();
}