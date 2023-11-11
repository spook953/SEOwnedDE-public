#pragma once

#include "../../../SDK/SDK.h"

enum class EWeaponType {
	HITSCAN, MELEE, PROJECTILE, OTHER
};

enum EWeaponSlots {
	WEAPON_SLOT_PRIMARY, WEAPON_SLOT_SECONDARY, WEAPON_SLOT_MELEE
};

class CTraceFilterHitscan : public CTraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask);
	virtual TraceType_t GetTraceType() const;

public:
	C_BaseEntity *m_pIgnore = nullptr;
};

class CTraceFilterWorldCustom : public CTraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask);
	virtual TraceType_t GetTraceType() const;

public:
	C_BaseEntity *m_pTarget = nullptr;
};

class CAimUtils
{
public:
	void Trace(const Vec3 &start, const Vec3 &end, unsigned int mask, CTraceFilter *filter, trace_t *trace);
	void TraceHull(const Vec3 &start, const Vec3 &end, const Vec3 &mins, const Vec3 &maxs, unsigned int mask, CTraceFilter *filter, trace_t *trace);
	bool TraceEntityBullet(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo, int *pHitHitboxOut = nullptr);
	bool TraceEntityAutoDet(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo);
	bool TraceProjectile(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo);
	bool TraceProjectilePipes(const Vec3 &vFrom, const Vec3 &vTo, C_BaseEntity *pTarget, bool *pHitTarget);
	bool TraceFlames(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo);
	bool TraceEntityMelee(C_BaseEntity *pEntity, const Vec3 &vFrom, const Vec3 &vTo);
	bool TracePositionWorld(const Vec3 &vFrom, const Vec3 &vTo);
	EWeaponType GetWeaponType(C_TFWeaponBase *pWeapon);
	void FixMovement(CUserCmd *pCmd, const Vec3 &vTargetAngle);
	bool IsWeaponCapableOfHeadshot(C_TFWeaponBase *pWeapon);
	void GetProjectileFireSetup(const Vec3 &vViewAngles, Vec3 vOffset, Vec3 *vSrc);
	bool IsBehindAndFacingTarget(const Vec3 &vPlayerCenter, const Vec3 &vTargetCenter, const Vec3 &vPlayerViewAngles, const Vec3 &vTargetEyeAngles);
};

MAKE_SINGLETON_SCOPED(CAimUtils, AimUtils, F);