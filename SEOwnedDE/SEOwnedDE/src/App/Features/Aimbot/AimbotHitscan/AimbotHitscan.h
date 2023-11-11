#pragma once

#include "../../LagRecords/LagRecords.h"

class CAimbotHitscan
{
private:
	struct Target_t
	{
		C_BaseEntity *m_pEntity = nullptr;
		Vec3 m_vPosition = {};
		Vec3 m_vAngleTo = {};
		float m_flFOVTo = 0.0f;
		float m_flDistanceTo = 0.0f;
		int m_nAimedHitbox = -1;
		float m_flSimulationTime = -1.0f;
		const LagRecord_t *m_pLagRecord = nullptr;
		bool m_bWasMultiPointed = false;
	};

	std::vector<Target_t> m_vecTargets = {};

private:
	int GetAimHitbox(C_TFWeaponBase *pWeapon);
	bool ScanHead(C_TFPlayer *pLocal, Target_t &Target);
	bool ScanBody(C_TFPlayer *pLocal, Target_t &Target);
	bool ScanBuilding(C_TFPlayer *pLocal, Target_t &Target);
	bool GetTarget(C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, Target_t &Out);
	bool ShouldAim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Target_t &Target);
	void Aim(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Vec3 &vAngles);
	bool ShouldFire(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon, const Target_t &Target);
	void HandleFire(CUserCmd *pCmd, C_TFWeaponBase *pWeapon);

public:
	bool IsFiring(CUserCmd *pCmd, C_TFWeaponBase *pWeapon);

public:
	void Run(CUserCmd *pCmd, C_TFPlayer *pLocal, C_TFWeaponBase *pWeapon);
};

MAKE_SINGLETON_SCOPED(CAimbotHitscan, AimbotHitscan, F);