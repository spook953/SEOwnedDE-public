#pragma once

#include "../../../SDK/SDK.h"

struct ProjectileInfo
{
	ProjectileType_t m_type{};

	Vec3 m_pos{};
	Vec3 m_ang{};

	float m_speed{};
	float m_gravity_mod{};

	bool no_spin{};
};

namespace projectile_simulation
{
	bool getInfo(C_TFPlayer *player, C_TFWeaponBase *weapon, const Vec3 &angles, ProjectileInfo &out);
	bool init(const ProjectileInfo &info, bool no_vec_up = false);
	void runTick();
	Vec3 getOrigin();
}