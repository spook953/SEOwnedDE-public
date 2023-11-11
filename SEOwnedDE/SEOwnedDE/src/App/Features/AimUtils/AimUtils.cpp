#include "AimUtils.h"

#include "../CFG.h"

bool CTraceFilterHitscan::ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask)
{
	auto pLocal = H::Entities->GetLocal();
	auto pWeapon = H::Entities->GetWeapon();

	if (!pLocal || !pWeapon)
		return false;

	if (!pServerEntity || pServerEntity == m_pIgnore || pServerEntity == pLocal)
		return false;

	if (auto pEntity = static_cast<IClientEntity *>(pServerEntity)->As<C_BaseEntity>())
	{
		switch (pEntity->GetClassId())
		{
			case ETFClassIds::CFuncAreaPortalWindow:
			case ETFClassIds::CFuncRespawnRoomVisualizer:
			case ETFClassIds::CSniperDot:
			case ETFClassIds::CTFAmmoPack: return false;

			case ETFClassIds::CTFMedigunShield:
			{
				if (pEntity->m_iTeamNum() == pLocal->m_iTeamNum())
					return false;

				break;
			}

			case ETFClassIds::CTFPlayer:
			case ETFClassIds::CObjectSentrygun:
			case ETFClassIds::CObjectDispenser:
			case ETFClassIds::CObjectTeleporter:
			{
				switch (pWeapon->GetWeaponID())
				{
					case TF_WEAPON_SNIPERRIFLE:
					case TF_WEAPON_SNIPERRIFLE_CLASSIC:
					case TF_WEAPON_SNIPERRIFLE_DECAP:
					{
						if (pEntity->m_iTeamNum() == pLocal->m_iTeamNum())
							return false;

						break;
					}

					default: break;
				}

				break;
			}

			default: break;
		}

	}

	return true;
}

TraceType_t CTraceFilterHitscan::GetTraceType() const
{
	return TRACE_EVERYTHING;
}

bool CTraceFilterWorldCustom::ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask)
{
	if (auto pEntity = static_cast<IClientEntity *>(pServerEntity)->As<C_BaseEntity>())
	{
		switch (pEntity->GetClassId())
		{
			case ETFClassIds::CTFPlayer:
			case ETFClassIds::CObjectSentrygun:
			case ETFClassIds::CObjectDispenser:
			case ETFClassIds::CObjectTeleporter: return pEntity == m_pTarget;

			case ETFClassIds::CObjectCartDispenser:
			case ETFClassIds::CBaseDoor:
			case ETFClassIds::CPhysicsProp:
			case ETFClassIds::CDynamicProp:
			case ETFClassIds::CBaseEntity:
			case ETFClassIds::CFuncTrackTrain: return true;

			default: return false;
		}
	}

	return false;
}

TraceType_t CTraceFilterWorldCustom::GetTraceType() const
{
	return TRACE_EVERYTHING;
}

void CAimUtils::Trace(const Vec3 &start, const Vec3 &end, unsigned int mask, CTraceFilter *filter, trace_t *trace)
{
	Ray_t ray = {};
	ray.Init(start, end);
	I::EngineTrace->TraceRay(ray, mask, filter, trace);
}

void CAimUtils::TraceHull(const Vec3 &start, const Vec3 &end, const Vec3 &mins, const Vec3 &maxs, unsigned int mask, CTraceFilter *filter, trace_t *trace)
{
	Ray_t ray = {};
	ray.Init(start, end, mins, maxs);
	I::EngineTrace->TraceRay(ray, mask, filter, trace);
}

bool CAimUtils::TraceEntityBullet(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo, int *pHitHitboxOut)
{
	trace_t trace = {};
	CTraceFilterHitscan filter = {};

	Trace(vFrom, vTo, (MASK_SHOT | CONTENTS_GRATE), &filter, &trace);

	if (trace.m_pEnt == pEntity && !trace.allsolid && !trace.startsolid)
	{
		if (pHitHitboxOut)
			*pHitHitboxOut = trace.hitbox;

		return true;
	}

	return false;
}

bool CAimUtils::TraceEntityAutoDet(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo)
{
	trace_t trace = {};
	CTraceFilterWorldCustom filter = {};
	filter.m_pTarget = pEntity;
	Trace(vFrom, vTo, MASK_SOLID, &filter, &trace);
	return trace.m_pEnt == pEntity || trace.fraction > 0.99f;
}

bool CAimUtils::TraceProjectile(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo)
{
	trace_t trace = {};
	CTraceFilterWorldCustom filter = {};
	filter.m_pTarget = pEntity;

	TraceHull(vFrom, vTo, { -4.0f, -4.0f, -4.0f }, { 4.0f, 4.0f, 4.0f }, MASK_SOLID, &filter, &trace);

	return trace.m_pEnt == pEntity || (trace.fraction > 0.99f && !trace.allsolid && !trace.startsolid);
}

bool CAimUtils::TraceProjectilePipes(const Vec3 &vFrom, const Vec3 &vTo, C_BaseEntity *pTarget, bool *pHitTarget)
{
	trace_t Trace = {};
	CTraceFilterWorldCustom Filter = {};
	Filter.m_pTarget = pTarget;

	TraceHull(vFrom, vTo, { -8.0f, -8.0f, -8.0f }, { 8.0f, 8.0f, 8.0f }, MASK_SOLID, &Filter, &Trace);

	if (Trace.m_pEnt == pTarget) {
		*pHitTarget = true;
		return false;
	}

	return Trace.fraction > 0.99f && !Trace.startsolid && !Trace.allsolid;
}

bool CAimUtils::TraceFlames(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo)
{
	trace_t trace = {};
	CTraceFilterWorldCustom filter = {};
	filter.m_pTarget = pEntity;

	TraceHull(vFrom, vTo, { -12.0f, -12.0f, -12.0f }, { 12.0f, 12.0f, 12.0f }, MASK_SOLID, &filter, &trace);

	return trace.m_pEnt == pEntity || (trace.fraction > 0.99f && !trace.allsolid && !trace.startsolid);
}

bool CAimUtils::TraceEntityMelee(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo)
{
	trace_t Trace = {};
	CTraceFilterHitscan Filter = {};

	TraceHull(vFrom, vTo, { -18.0f, -18.0f, -18.0f }, { 18.0f, 18.0f, 18.0f }, MASK_SOLID, &Filter, &Trace);

	return Trace.m_pEnt == pEntity;
}

bool CAimUtils::TracePositionWorld(const Vec3 &vFrom, const Vec3 &vTo)
{
	trace_t trace = {};
	CTraceFilterWorldCustom filter = {};

	Trace(vFrom, vTo, MASK_SOLID, &filter, &trace);

	return trace.fraction > 0.99f && !trace.allsolid && !trace.startsolid;
}

EWeaponType CAimUtils::GetWeaponType(C_TFWeaponBase *pWeapon)
{
	if (!pWeapon)
	{
		return EWeaponType::OTHER;
	}

	if (pWeapon->GetSlot() == WEAPON_SLOT_MELEE)
	{
		return EWeaponType::MELEE;
	}

	switch (pWeapon->GetWeaponID())
	{
		case TF_WEAPON_ROCKETLAUNCHER:
		case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
		case TF_WEAPON_GRENADELAUNCHER:
		case TF_WEAPON_PIPEBOMBLAUNCHER:
		case TF_WEAPON_FLAREGUN:
		case TF_WEAPON_COMPOUND_BOW:
		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_PARTICLE_CANNON:
		case TF_WEAPON_DRG_POMSON:
		case TF_WEAPON_RAYGUN:
		case TF_WEAPON_FLAREGUN_REVENGE:
		case TF_WEAPON_CANNON:
		case TF_WEAPON_SYRINGEGUN_MEDIC:
		case TF_WEAPON_FLAME_BALL:
		case TF_WEAPON_FLAMETHROWER:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
		{
			return EWeaponType::PROJECTILE;
		}

		case TF_WEAPON_BUILDER:
		case TF_WEAPON_ROCKETPACK:
		case TF_WEAPON_MEDIGUN:
		{
			return EWeaponType::OTHER;
		}

		default:
		{
			int nDamageType = pWeapon->GetDamageType();

			if (nDamageType & DMG_BULLET || nDamageType & DMG_BUCKSHOT)
			{
				return EWeaponType::HITSCAN;
			}

			break;
		}
	}

	return EWeaponType::OTHER;
}

#pragma warning (push)
#pragma warning (disable : 4244)
#pragma warning (disable : 26451)

void CAimUtils::FixMovement(CUserCmd *pCmd, const Vec3 &vTargetAngle)
{
	Vec3 vMove(pCmd->forwardmove, pCmd->sidemove, pCmd->upmove);
	Vec3 vMoveDir = {};

	Math::VectorAngles(vMove, vMoveDir);

	float flSpeed = Math::FastSqrt(vMove.x * vMove.x + vMove.y * vMove.y);
	float flYaw = DEG2RAD(vTargetAngle.y - pCmd->viewangles.y + vMoveDir.y);

	pCmd->forwardmove = cos(flYaw) * flSpeed;
	pCmd->sidemove = sin(flYaw) * flSpeed;
}

#pragma warning (pop)

bool CAimUtils::IsWeaponCapableOfHeadshot(C_TFWeaponBase *pWeapon)
{
	auto pOwner = pWeapon->m_hOwnerEntity().Get();

	if (!pOwner)
		return false;

	bool bMaybe = false, bIsSniperRifle = false, bIsRevolver = false;

	switch (pWeapon->GetWeaponID())
	{
		case TF_WEAPON_COMPOUND_BOW: return true;
		case TF_WEAPON_SNIPERRIFLE:
		case TF_WEAPON_SNIPERRIFLE_CLASSIC:
		case TF_WEAPON_SNIPERRIFLE_DECAP: bMaybe = bIsSniperRifle = pOwner->As<C_TFPlayer>()->IsZoomed(); break;
		case TF_WEAPON_REVOLVER: bMaybe = bIsRevolver = true; break;
		default: break;
	}

	if (bMaybe)
	{
		int nWeaponMode = static_cast<int>(SDKUtils::AttribHookValue(0.0f, "set_weapon_mode", pWeapon));

		if (bIsSniperRifle)
			return nWeaponMode != 1;

		else if (bIsRevolver)
			return nWeaponMode == 1;
	}

	return false;
}

void CAimUtils::GetProjectileFireSetup(const Vec3 &vViewAngles, Vec3 vOffset, Vec3 *vSrc)
{
	static ConVar *cl_flipviewmodels = I::CVar->FindVar("cl_flipviewmodels");

	if (cl_flipviewmodels && cl_flipviewmodels->GetInt())
		vOffset.y *= -1.0f;

	Vec3 vForward = {}, vRight = {}, vUp = {};
	Math::AngleVectors(vViewAngles, &vForward, &vRight, &vUp);

	*vSrc += (vForward * vOffset.x) + (vRight * vOffset.y) + (vUp * vOffset.z);
}

bool CAimUtils::IsBehindAndFacingTarget(const Vec3 &vPlayerCenter, const Vec3 &vTargetCenter, const Vec3 &vPlayerViewAngles, const Vec3 &vTargetEyeAngles)
{
	Vec3 vToTarget = {};
	vToTarget = vTargetCenter - vPlayerCenter;
	vToTarget.z = 0.0f;
	vToTarget.NormalizeInPlace();

	Vec3 vPlayerForward = {};
	Math::AngleVectors(vPlayerViewAngles, &vPlayerForward);
	vPlayerForward.z = 0.0f;
	vPlayerForward.NormalizeInPlace();

	Vec3 vTargetForward = {};
	Math::AngleVectors(vTargetEyeAngles, &vTargetForward);
	vTargetForward.z = 0.0f;
	vTargetForward.NormalizeInPlace();

	float flPosVsTargetViewDot = vToTarget.Dot(vTargetForward);
	float flPosVsOwnerViewDot = vToTarget.Dot(vPlayerForward);
	float flViewAnglesDot = vTargetForward.Dot(vPlayerForward);

	return flPosVsTargetViewDot > 0.0f && flPosVsOwnerViewDot > 0.5f && flViewAnglesDot > -0.3f;
}