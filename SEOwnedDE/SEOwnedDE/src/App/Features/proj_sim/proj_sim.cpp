#include "proj_sim.hpp"

ProjInfo cur_proj{};

Vec3 cur_vel{};
float cur_drag{};

bool proj_sim::getInfo(C_TFPlayer *player, C_TFWeaponBase *weapon, const Vec3 &angles, ProjInfo &out)
{
	if (!player || !weapon)
	{
		return false;
	}

	auto cur_time{ static_cast<float>(player->m_nTickBase()) * TICK_INTERVAL };
	auto ducking{ player->m_fFlags() & FL_DUCKING };

	Vec3 pos{};
	Vec3 ang{};

	switch (weapon->GetWeaponID())
	{
		case TF_WEAPON_GRENADELAUNCHER:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 16.0f, 8.0f, -6.0f }, angles, pos, ang, true);

			auto speed{ weapon->m_iItemDefinitionIndex() == Demoman_m_TheLochnLoad ? 1490.0f : 1200.0f };

			out = { TF_PROJECTILE_PIPEBOMB, pos, ang, std::min(speed, k_flMaxVelocity), 1.0f};

			return true;
		}

		case TF_WEAPON_PIPEBOMBLAUNCHER:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 16.0f, 8.0f, -6.0f }, angles, pos, ang, true);

			auto charge_begin_time{ weapon->As<C_TFPipebombLauncher>()->m_flChargeBeginTime() };
			auto charge{ cur_time - charge_begin_time };
			auto speed{ Math::RemapValClamped(charge, 0.0f, SDKUtils::AttribHookValue(4.0f, "stickybomb_charge_rate", weapon), 900.0f, 2400.0f) };

			if (charge_begin_time <= 0.0f)
			{
				speed = 900.0f;
			}

			out = { TF_PROJECTILE_PIPEBOMB_REMOTE, pos, ang, std::min(speed, k_flMaxVelocity), 1.0f };

			return true;
		}

		case TF_WEAPON_CANNON:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 16.0f, 8.0f, -6.0f }, angles, pos, ang, true);

			out = { TF_PROJECTILE_CANNONBALL, pos, ang, std::min(1454.0f, k_flMaxVelocity), 1.0f };

			return true;
		}

		case TF_WEAPON_FLAREGUN:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 23.5f, 12.0f, ducking ? 8.0f : -3.0f }, angles, pos, ang, false);

			out = { TF_PROJECTILE_FLARE, pos, ang, 2000.0f, 0.3f };

			return true;
		}

		case TF_WEAPON_FLAREGUN_REVENGE:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 23.5f, 12.0f, ducking ? 8.0f : -3.0f }, angles, pos, ang, false);

			out = { TF_PROJECTILE_FLARE, pos, ang, 3000.0f, 0.45f };

			return true;
		}

		case TF_WEAPON_COMPOUND_BOW:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 23.5f, 8.0f, -3.0f }, angles, pos, ang, false);

			auto charge_begin_time{ weapon->As<C_TFPipebombLauncher>()->m_flChargeBeginTime() };
			auto charge{ cur_time - charge_begin_time };
			auto speed{ Math::RemapValClamped(charge, 0.0f, 1.0f, 1800.0f, 2600.0f) };
			auto grav_mod{ Math::RemapValClamped(charge, 0.0f, 1.0f, 0.5f, 0.1f) };

			if (charge_begin_time <= 0.0f)
			{
				speed = 1800.0f;
				grav_mod = 0.5f;
			}

			out = { TF_PROJECTILE_ARROW, pos, ang, speed, grav_mod };

			return true;
		}

		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 23.5f, 8.0f, -3.0f }, angles, pos, ang, false);

			out = { TF_PROJECTILE_ARROW, pos, ang, 2400.0f, 0.2f };

			return true;
		}

		case TF_WEAPON_SYRINGEGUN_MEDIC:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 16.0f, 6.0f, -8.0f }, angles, pos, ang, false);

			out = { TF_PROJECTILE_SYRINGE, pos, ang, 1000.0f, 0.3f };

			return true;

			return true;
		}

		default:
		{
			return false;
		}
	}
}

bool proj_sim::init(const ProjInfo &info)
{
	cur_proj = info;
	cur_vel = {};
	cur_drag = 0.0f;

	Vec3 forward{}, up{};

	Math::AngleVectors(cur_proj.m_ang, &forward, nullptr, &up);

	cur_vel = forward * cur_proj.m_speed;
	
	static auto tf_mvm_bot_flag_carrier_health_regen{ I::CVar->FindVar("tf_mvm_bot_flag_carrier_health_regen") };

	if (cur_proj.m_type == TF_PROJECTILE_PIPEBOMB)
	{
		cur_vel += up * 200.0f;
		cur_drag = tf_mvm_bot_flag_carrier_health_regen->GetFloat();
	}

	if (cur_proj.m_type == TF_PROJECTILE_PIPEBOMB_REMOTE)
	{
		cur_vel += up * 200.0f;
		cur_drag = tf_mvm_bot_flag_carrier_health_regen->GetFloat();
	}

	if (cur_proj.m_type == TF_PROJECTILE_PIPEBOMB_PRACTICE)
	{
		cur_vel += up * 200.0f;
		cur_drag = tf_mvm_bot_flag_carrier_health_regen->GetFloat();
	}

	if (cur_proj.m_type == TF_PROJECTILE_CANNONBALL)
	{
		cur_vel += up * 200.0f;
		cur_drag = tf_mvm_bot_flag_carrier_health_regen->GetFloat();
	}

	return true;
}

void proj_sim::runTick()
{
	//gravity
	cur_vel.z += -(800.0f * cur_proj.m_gravity_mod) * TICK_INTERVAL;

	//drag
	cur_vel += cur_vel * (-0.5f * cur_drag * 2.0f * TICK_INTERVAL);

	cur_proj.m_pos += cur_vel * TICK_INTERVAL;
}

Vec3 proj_sim::getOrigin()
{
	return cur_proj.m_pos;
}