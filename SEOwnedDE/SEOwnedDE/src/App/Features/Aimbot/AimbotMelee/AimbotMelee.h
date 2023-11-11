#pragma once

#include "../../LagRecords/LagRecords.h"

class CAimbotMelee
{
private:
	struct Target_t
	{
		C_BaseEntity *m_pEntity = nullptr;
		Vec3 m_vPosition = {};
		Vec3 m_vAngleTo = {};
		float m_flFOVTo = 0.0f;
		float m_flDistanceTo = 0.0f;
		float m_flSimulationTime = -1.0f;
		const LagRecord_t *m_pLagRecord = nullptr;
		bool m_bMeleeTraceHit = false;
	};

	std::vector<Target_t> m_vecTargets = {};

private:
	bool CanSee(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, Target_t &Target);

private:
	bool GetTarget(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, Target_t &Out);
	bool ShouldAim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Target_t &Target);
	void Aim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Vec3 &vAngles);
	bool ShouldFire(Target_t &Target);
	void HandleFire(CUserCmd *pCmd, C_TFWeaponBase *pWeapon, const Target_t &Target);

public:
	bool IsFiring(CUserCmd *pCmd, C_TFWeaponBase *pWeapon);

public:
	void Run(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon);
};

MAKE_SINGLETON_SCOPED(CAimbotMelee, AimbotMelee, F);