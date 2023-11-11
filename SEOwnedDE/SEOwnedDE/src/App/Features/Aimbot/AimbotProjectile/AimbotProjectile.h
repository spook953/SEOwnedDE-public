#pragma once

#include "../../../../SDK/SDK.h"

class CAimbotProjectile
{
private:
	struct Target_t
	{
		C_BaseEntity *m_pEntity = nullptr;
		Vec3 m_vPosition = {};
		Vec3 m_vAngleTo = {};
		float m_flFOVTo = 0.0f;
		float m_flDistanceTo = 0.0f;
		float m_flTimeToTarget = 0.0f;
	};

	std::vector<Target_t> m_vecTargets = {};

private:
	struct ProjectileInfo_t
	{
		float m_flSpeed = 0.0f;
		float m_flGravityMod = 0.0f;
		bool m_bPipes = false;
		bool m_bFlamethrower{};
	};

	ProjectileInfo_t m_CurProjInfo = {};

private:
	bool GetProjectileInfo(C_TFWeaponBase *pWeapon);
	bool CalcProjAngle(const Vec3 vFrom, const Vec3 &vTo, Vec3 &vAngleOut, float &flTimeOut);
	void OffsetPlayerPosition(C_TFWeaponBase *pWeapon, Vec3 &vPos, C_TFPlayer *pPlayer, bool bDucked, bool bOnGround);
	bool CanArcReach(const Vec3 &vFrom, const Vec3 &vTo, const Vec3 &vAngleTo, float flTargetTime, C_BaseEntity *pTarget);
	bool CanSee(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Vec3 &vFrom, const Vec3 &vTo, const Target_t &Target, float flTargetTime);
	bool SolveTarget(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, CUserCmd *pCmd, Target_t &Target);

private:
	bool GetTarget(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, CUserCmd *pCmd, Target_t &Out);
	bool ShouldAim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon);
	void Aim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Vec3 &vAngles);
	bool ShouldFire(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon);
	void HandleFire(CUserCmd *pCmd, C_TFWeaponBase *pWeapon, C_TFPlayer *pLocal, const Target_t &Target);

public:
	bool IsFiring(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon);

public:
	void Run(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon);
};

MAKE_SINGLETON_SCOPED(CAimbotProjectile, AimbotProjectile, F);