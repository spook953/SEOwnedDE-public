#include "ProjectileSimulation.h"

IPhysicsEnvironment *env{};
IPhysicsObject *obj{};

bool projectile_simulation::getInfo(C_TFPlayer *player, C_TFWeaponBase *weapon, const Vec3 &angles, ProjectileInfo &out)
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

			auto is_lochnload{ weapon->m_iItemDefinitionIndex() == Demoman_m_TheLochnLoad };
			auto speed{ SDKUtils::AttribHookValue(1200.0, "mult_projectile_speed", weapon) };

			out = { TF_PROJECTILE_PIPEBOMB, pos, ang, speed, 1.0f, is_lochnload };

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

			out = { TF_PROJECTILE_PIPEBOMB_REMOTE, pos, ang, speed, 1.0f, false };

			return true;
		}

		case TF_WEAPON_CANNON:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 16.0f, 8.0f, -6.0f }, angles, pos, ang, true);

			out = { TF_PROJECTILE_CANNONBALL, pos, ang, 1454.0f, 1.0f, false };

			return true;
		}

		case TF_WEAPON_FLAREGUN:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 23.5f, 12.0f, ducking ? 8.0f : -3.0f }, angles, pos, ang, false);

			out = { TF_PROJECTILE_FLARE, pos, ang, 2000.0f, 0.3f, true };

			return true;
		}

		case TF_WEAPON_FLAREGUN_REVENGE:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 23.5f, 12.0f, ducking ? 8.0f : -3.0f }, angles, pos, ang, false);

			out = { TF_PROJECTILE_FLARE, pos, ang, 3000.0f, 0.45f, true };

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

			out = { TF_PROJECTILE_ARROW, pos, ang, speed, grav_mod, true };

			return true;
		}

		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 23.5f, 8.0f, -3.0f }, angles, pos, ang, false);

			out = { TF_PROJECTILE_ARROW, pos, ang, 2400.0f, 0.2f, true };

			return true;
		}

		case TF_WEAPON_SYRINGEGUN_MEDIC:
		{
			SDKUtils::GetProjectileFireSetupRebuilt(player, { 16.0f, 6.0f, -8.0f }, angles, pos, ang, false);

			out = { TF_PROJECTILE_SYRINGE, pos, ang, 1000.0f, 0.3f, true };

			return true;

			return true;
		}

		default:
		{
			return false;
		}
	}
}

bool projectile_simulation::init(const ProjectileInfo &info, bool no_vec_up)
{
	if (!env)
	{
		env = I::Physics->CreateEnvironment();
	}

	if (!obj)
	{
		//it doesn't matter what the size is for non drag affected projectiles
		//pipes use the size below so it works out just fine
		auto col{ I::PhysicsCollision->BBoxToCollide({ -2.0f, -2.0f, -2.0f }, { 2.0f, 2.0f, 2.0f }) };

		auto params{ g_PhysDefaultObjectParams };

		params.damping = 0.0f;
		params.rotdamping = 0.0f;
		params.inertia = 0.0f;
		params.rotInertiaLimit = 0.0f;
		params.enableCollisions = false;

		obj = env->CreatePolyObject(col, 0, info.m_pos, info.m_ang, &params);

		obj->Wake();
	}

	if (!env || !obj)
	{
		return false;
	}

	//set position and velocity
	{
		Vec3 forward{}, up{};

		Math::AngleVectors(info.m_ang, &forward, nullptr, &up);

		Vec3 vel{ forward * info.m_speed };
		Vec3 ang_vel{};

		switch (info.m_type)
		{
			case TF_PROJECTILE_PIPEBOMB:
			case TF_PROJECTILE_PIPEBOMB_REMOTE:
			case TF_PROJECTILE_PIPEBOMB_PRACTICE:
			case TF_PROJECTILE_CANNONBALL:
			{
				//CTFWeaponBaseGun::FirePipeBomb
				//pick your poison

				if (!no_vec_up)
				{
					vel += up * 200.0f;
				}

				ang_vel = { 600.0f, -1200.0f, 0.0f};

				break;
			}

			default:
			{
				break;
			}
		}

		if (info.no_spin)
		{
			ang_vel.Zero();
		}

		obj->SetPosition(info.m_pos, info.m_ang, true);
		obj->SetVelocity(&vel, &ang_vel);
	}

	//set drag
	{
		float drag{};
		Vec3 drag_basis{};
		Vec3 ang_drag_basis{};

		//these values were dumped from the server by firing the projectiles with 0 0 0 angles
		//they are calculated in CPhysicsObject::RecomputeDragBases
		switch (info.m_type)
		{
			case TF_PROJECTILE_PIPEBOMB:
			{
				drag = 1.0f;
				drag_basis = { 0.003902f, 0.009962f, 0.009962f };
				ang_drag_basis = { 0.003618f, 0.001514f, 0.001514f };

				break;
			}

			case TF_PROJECTILE_PIPEBOMB_REMOTE:
			case TF_PROJECTILE_PIPEBOMB_PRACTICE:
			{
				drag = 1.0f;
				drag_basis = { 0.007491f, 0.007491f, 0.007306f };
				ang_drag_basis = { 0.002777f, 0.002842f, 0.002812f };

				break;
			}

			case TF_PROJECTILE_CANNONBALL:
			{
				drag = 1.0f;
				drag_basis = { 0.020971f, 0.019420f, 0.020971f };
				ang_drag_basis = { 0.012997f, 0.013496f, 0.013714f };

				break;
			}

			default:
			{
				break;
			}
		}

		obj->SetDragCoefficient(&drag, &drag);

		obj->m_dragBasis = drag_basis;
		obj->m_angDragBasis = ang_drag_basis;
	}

	//set env params
	{	
		auto max_vel{ 1000000.0f };
		auto max_ang_vel{ 1000000.0f };

		//only pipes need k_flMaxVelocity and k_flMaxAngularVelocity
		switch (info.m_type)
		{
			case TF_PROJECTILE_PIPEBOMB:
			case TF_PROJECTILE_PIPEBOMB_REMOTE:
			case TF_PROJECTILE_PIPEBOMB_PRACTICE:
			case TF_PROJECTILE_CANNONBALL:
			{
				max_vel = k_flMaxVelocity;
				max_ang_vel = k_flMaxAngularVelocity;

				break;
			}

			default:
			{
				break;
			}
		}

		physics_performanceparams_t params{};
		params.Defaults();

		params.maxVelocity = max_vel;
		params.maxAngularVelocity = max_ang_vel;

		env->SetPerformanceSettings(&params);
		env->SetAirDensity(2.0f);
		env->SetGravity({ 0.0f, 0.0f, -(800.0f * info.m_gravity_mod) });

		env->ResetSimulationClock(); //not needed?
	}

	return true;
}

void projectile_simulation::runTick()
{
	if (!env)
	{
		return;
	}

	env->Simulate(TICK_INTERVAL);
}

Vec3 projectile_simulation::getOrigin()
{
	if (!obj)
	{
		return {};
	}

	Vec3 out{};

	obj->GetPosition(&out, nullptr);

	return out;
}