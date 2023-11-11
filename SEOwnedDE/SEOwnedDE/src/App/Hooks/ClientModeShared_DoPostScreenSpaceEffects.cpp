#include "../../SDK/SDK.h"

#include "../Features/CFG.h"
#include "../Features/AimUtils/AimUtils.h"
#include "../Features/VisualUtils/VisualUtils.h"
#include "../Features/ProjectileSimulation/ProjectileSimulation.h"

void RenderLine(const Vector &v1, const Vector &v2, Color_t c, bool bZBuffer)
{
	reinterpret_cast<void(__cdecl *)(const Vector &, const Vector &, Color_t, bool)>
		(Signatures::RenderLine.Get())(v1, v2, c, bZBuffer);
}

void RenderBox(const Vector &origin, const QAngle &angles, const Vector &mins, const Vector &maxs, Color_t c, bool bZBuffer, bool bInsideOut = false)
{
	reinterpret_cast<void(__cdecl *)(const Vector &, const QAngle &, const Vector &, const Vector &, Color_t c, bool, bool)>
		(Memory::RelToAbs(Signatures::RenderBox.Get()))(origin, angles, mins, maxs, c, bZBuffer, bInsideOut);
}

void RenderWireframeBox(const Vector &vOrigin, const QAngle &angles, const Vector &vMins, const Vector &vMaxs, Color_t c, bool bZBuffer)
{
	reinterpret_cast<void(__cdecl *)(const Vector &, const QAngle &, const Vector &, const Vector &, Color_t c, bool)>
		(Signatures::RenderWireframeBox.Get())(vOrigin, angles, vMins, vMaxs, c, bZBuffer);
}

void SniperLines()
{
	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	auto GetMaxViewOffsetZ = [&](C_TFPlayer *pPlayer)
	{
		if (pPlayer->m_fFlags() & FL_DUCKING)
			return 45.0f;

		switch (pPlayer->m_iClass())
		{
			case TF_CLASS_SCOUT: return 65.0f;
			case TF_CLASS_SOLDIER: return 68.0f;
			case TF_CLASS_PYRO: return 68.0f;
			case TF_CLASS_DEMOMAN: return 68.0f;
			case TF_CLASS_HEAVYWEAPONS: return 75.0f;
			case TF_CLASS_ENGINEER: return 68.0f;
			case TF_CLASS_MEDIC: return 75.0f;
			case TF_CLASS_SNIPER: return 75.0f;
			case TF_CLASS_SPY: return 75.0f;
			default: return 0.0f;
		}
	};

	if (!CFG::ESP_Active || !CFG::ESP_Players_Active || !CFG::ESP_Players_Sniper_Lines
		|| I::EngineVGui->IsGameUIVisible() || SDKUtils::BInEndOfMatch() || G::bRenderingSpyCamera)
		return;

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal)
		return;

	for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ALL))
	{
		if (!pEntity)
			continue;

		auto pPlayer = pEntity->As<C_TFPlayer>();

		if (!pPlayer || pPlayer == pLocal || pPlayer->deadflag() || pPlayer->m_iClass() != TF_CLASS_SNIPER)
			continue;

		auto weapon{ pPlayer->m_hActiveWeapon().Get() };

		if (!weapon)
		{
			continue;
		}

		bool classic_charging{ weapon->As<C_TFWeaponBase>()->m_iItemDefinitionIndex() == Sniper_m_TheClassic && weapon->As<C_TFSniperRifleClassic>()->m_bCharging() };

		if (!pPlayer->InCond(TF_COND_ZOOMED) && !classic_charging)
		{
			continue;
		}

		bool bIsFriend = pPlayer->IsPlayerOnSteamFriendsList();

		if (CFG::ESP_Players_Ignore_Friends && bIsFriend)
			continue;

		if (!bIsFriend)
		{
			if (CFG::ESP_Players_Ignore_Teammates && pPlayer->m_iTeamNum() == pLocal->m_iTeamNum())
				continue;

			if (CFG::ESP_Players_Ignore_Enemies && pPlayer->m_iTeamNum() != pLocal->m_iTeamNum())
				continue;
		}

		Vec3 vForward = {};
		Math::AngleVectors(pPlayer->GetEyeAngles(), &vForward);

		Vec3 vStart = pPlayer->m_vecOrigin() + Vec3(0.0f, 0.0f, GetMaxViewOffsetZ(pPlayer));
		Vec3 vEnd = vStart + (vForward * 8192.0f);

		CTraceFilterWorldCustom Filter = {};
		trace_t Trace = {};

		F::AimUtils->Trace(vStart, vEnd, MASK_SOLID, &Filter, &Trace);

		vEnd = Trace.endpos;

		RenderLine(vStart, vEnd, F::VisualUtils->GetEntityColor(pLocal, pPlayer), true);
	}
}

void projectileArc()
{
	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	if (I::Input->CAM_IsThirdPerson())
	{
		return;
	}

	class CTraceFilterArc : public CTraceFilter
	{
	public:
		virtual bool ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask)
		{
			if (auto pEntity = static_cast<IClientEntity *>(pServerEntity)->As<C_BaseEntity>())
			{
				switch (pEntity->GetClassId())
				{
					case ETFClassIds::CTFPlayer:
					case ETFClassIds::CObjectSentrygun:
					case ETFClassIds::CObjectDispenser:
					case ETFClassIds::CObjectTeleporter:
					case ETFClassIds::CObjectCartDispenser:
					case ETFClassIds::CBaseDoor:
					case ETFClassIds::CPhysicsProp:
					case ETFClassIds::CDynamicProp:
					case ETFClassIds::CBaseEntity:
					case ETFClassIds::CFuncTrackTrain:
					{
						return true;
					}

					default:
					{
						return false;
					}
				}
			}

			return false;
		}

		virtual TraceType_t GetTraceType() const
		{
			return TRACE_EVERYTHING;
		}
	};

	if (!CFG::Visuals_Draw_Projectile_Arc || !G::CHudTFCrosshair_ShouldDraw_Result)
	{
		return;
	}

	auto local{ H::Entities->GetLocal() };

	if (!local)
	{
		return;
	}

	auto weapon{ H::Entities->GetWeapon() };

	if (!weapon)
	{
		return;
	}

	ProjectileInfo info{};

	auto backup_origin{ local->m_vecOrigin() };

	local->m_vecOrigin() = local->GetAbsOrigin();

	if (!projectile_simulation::getInfo(local, weapon, I::EngineClient->GetViewAngles(), info))
	{
		local->m_vecOrigin() = backup_origin;

		return;
	}

	local->m_vecOrigin() = backup_origin;

	if (!projectile_simulation::init(info))
	{
		return;
	}

	CTraceFilterArc filter{};

	auto max_time{ 5.0f };

	if (info.m_type == TF_PROJECTILE_PIPEBOMB)
	{
		max_time = weapon->m_iItemDefinitionIndex() == Demoman_m_TheIronBomber ? 1.54f : 2.2f;
	}

	if (info.m_type == TF_PROJECTILE_CANNONBALL)
	{
		max_time = 1.0f;
	}

	for (auto n{ 0 }; n < TIME_TO_TICKS(max_time); n++)
	{
		auto pre{ projectile_simulation::getOrigin() };

		projectile_simulation::runTick();

		auto post{ projectile_simulation::getOrigin() };

		auto clr{ F::VisualUtils->RainbowTickOffset(n) };

		if (CFG::Visuals_Draw_Projectile_Arc_Color_Mode == 0)
		{
			clr = CFG::Color_Projectile_Arc;
		}

		trace_t trace{};

		F::AimUtils->TraceHull(pre, post, { -2.0f, -2.0f, -2.0f }, { 2.0f, 2.0f, 2.0f }, MASK_SOLID, &filter, &trace);

		if (trace.DidHit())
		{
			Vec3 angles{};

			Math::VectorAngles(trace.plane.normal, angles);

			RenderBox(trace.endpos, angles, { -1.0f, -10.0f, -10.0f }, { 1.0f, 10.0f, 10.0f }, { clr.r, clr.g, clr.b, 50 }, false);
			RenderWireframeBox(trace.endpos, angles, { -1.0f, -10.0f, -10.0f }, { 1.0f, 10.0f, 10.0f }, clr, true);

			break;
		}

		RenderLine(pre, post, clr, false);
	}
}

MAKE_HOOK(
	ClientModeShared_DoPostScreenSpaceEffects, Memory::GetVFunc(I::ClientModeShared, 39),
	bool, __fastcall, void *ecx, void *edx, const CViewSetup *pSetup)
{
	auto original{ CALL_ORIGINAL(ecx, edx, pSetup) };

	SniperLines();
	projectileArc();

	return original;
}