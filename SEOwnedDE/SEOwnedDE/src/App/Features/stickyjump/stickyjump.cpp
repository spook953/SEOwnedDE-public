#include "stickyjump.hpp"

#include "../MovementSimulation/MovementSimulation.h"

#include "../CFG.h"

void walkTo(CUserCmd *cmd, const Vec3 &from, const Vec3 &to, float scale)
{
	auto delta{ to - from };

	if (delta.Length() == 0.0f)
	{
		return;
	}

	Vec3 delta_dir{};

	Math::VectorAngles({ delta.x, delta.y, 0.0f }, delta_dir);

	auto yaw{ DEG2RAD(delta_dir.y - cmd->viewangles.y) };

	cmd->forwardmove = cosf(yaw) * (450.0f * scale);
	cmd->sidemove = -sinf(yaw) * (450.0f * scale);
}

void stickyjump::run(CUserCmd *cmd)
{
	if (!H::Input->IsDown(CFG::Misc_Auto_Air_Pogo_Key)
		|| I::EngineVGui->IsGameUIVisible()
		|| I::MatSystemSurface->IsCursorVisible()
		|| SDKUtils::BInEndOfMatch())
	{
		return;
	}

	auto local{ H::Entities->GetLocal() };

	if (!local || local->deadflag() || (local->m_fFlags() & FL_ONGROUND) || !local->InCond(TF_COND_BLASTJUMPING))
	{
		return;
	}

	auto weapon{ H::Entities->GetWeapon() };

	if (!weapon || weapon->GetWeaponID() != TF_WEAPON_PIPEBOMBLAUNCHER)
	{
		return;
	}

	if (!F::MovementSimulation->Initialize(local))
	{
		return;
	}

	auto sim_ticks{ TIME_TO_TICKS(SDKUtils::AttribHookValue(0.8f, "sticky_arm_time", local)) };

	for (auto n{ 0 }; n < sim_ticks; n++)
	{
		F::MovementSimulation->RunTick();
	}

	auto end{ F::MovementSimulation->GetOrigin() };

	F::MovementSimulation->Restore();

	if (G::bFiring)
	{
		G::bSilentAngles = true;

		auto pitch_offset{ Math::RemapValClamped(I::EngineClient->GetViewAngles().x, 0.0f, -25.0f, 0.0f, -4.0f) };

		if (!(local->m_fFlags() & FL_DUCKING))
		{
			pitch_offset = -3.0f;
		}

		cmd->viewangles = Math::CalcAngle(local->GetShootPos(), end) + Vec3(pitch_offset, 0.0f, 0.0f);
	}

	if (H::Entities->GetGroup(EEntGroup::PROJECTILES_LOCAL_STICKIES).empty() && local->m_vecVelocity().z < 500.0f)
	{
		if (weapon->As<C_TFPipebombLauncher>()->m_flChargeBeginTime() > 0.0f)
		{
			cmd->buttons &= ~IN_ATTACK;

			//I::EngineClient->ClientCmd_Unrestricted("impulse 101");
		}

		else
		{
			cmd->buttons |= IN_ATTACK;
		}
	}

	for (auto ent : H::Entities->GetGroup(EEntGroup::PROJECTILES_LOCAL_STICKIES))
	{
		if (!ent)
		{
			continue;
		}

		auto sticky{ ent->As<C_TFGrenadePipebombProjectile>() };

		if (!sticky || sticky->m_bTouched())
		{
			continue;
		}

		cmd->forwardmove *= 0.05f;
		cmd->sidemove *= 0.05f;

		Vec3 sticky_vel{};

		sticky->EstimateAbsVelocity(sticky_vel);

		if (sticky->m_vecOrigin().DistTo(local->m_vecOrigin()) < 350.0f && local->m_vecVelocity().Length2D() > (sticky_vel.Length2D() * 1.05f))
		{
			cmd->buttons |= IN_ATTACK2;
		}

		auto slowdown_speed{ Math::RemapValClamped(I::EngineClient->GetViewAngles().x, 0.0f, -25.0f, 880.0f, 840.0f) };

		if (!(local->m_fFlags() & FL_DUCKING))
		{
			slowdown_speed *= 1.01f;
		}

		if (local->m_vecVelocity().Length2D() > slowdown_speed)
		{
			Vec3 forward{};

			Math::AngleVectors(Math::VelocityToAngles({ local->m_vecVelocity().x, local->m_vecVelocity().y, 0.0f }), &forward);

			walkTo(cmd, local->m_vecOrigin(), local->m_vecOrigin() + ((forward * -1.0f) * 100.0f), 1.0f);
		}

		break;
	}
}

//MAKE_SIGNATURE(CTFPlayer_PainSound, "server.dll", "E8 ? ? ? ? 8B 07 8B CF 8B 80 ? ? ? ? FF D0 89 45 DC", 0);
//
//MAKE_HOOK(
//	CTFPlayer_PainSound, Memory::RelToAbs(Signatures::CTFPlayer_PainSound.Get()),
//	void, __fastcall, void *ecx, void *edx, void *info)
//{
//}