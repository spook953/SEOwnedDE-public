#pragma once
#include "c_baseanimating.h"

enum
{
	TF_GL_MODE_REGULAR = 0,
	TF_GL_MODE_REMOTE_DETONATE,
	TF_GL_MODE_REMOTE_DETONATE_PRACTICE,
	TF_GL_MODE_CANNONBALL
};

class C_BaseProjectile : public C_BaseAnimating
{
public:
	NETVAR(m_hOriginalLauncher, EHANDLE, "CBaseProjectile", "m_hOriginalLauncher");
};

class C_BaseGrenade : public C_BaseProjectile
{
public:
	NETVAR(m_flDamage, float, "CBaseGrenade", "m_flDamage");
	NETVAR(m_DmgRadius, float, "CBaseGrenade", "m_DmgRadius");
	NETVAR(m_bIsLive, bool, "CBaseGrenade", "m_bIsLive");
	NETVAR(m_hThrower, EHANDLE, "CBaseGrenade", "m_hThrower");
	NETVAR(m_vecVelocity, Vec3, "CBaseGrenade", "m_vecVelocity");
	NETVAR(m_fFlags, int, "CBaseGrenade", "m_fFlags");
};

class C_TFBaseRocket : public C_BaseAnimating
{
public:
	NETVAR(m_vInitialVelocity, Vec3, "CTFBaseRocket", "m_vInitialVelocity");
	NETVAR(m_vecOrigin, Vec3, "CTFBaseRocket", "m_vecOrigin");
	NETVAR(m_angRotation, Vec3, "CTFBaseRocket", "m_angRotation");
	NETVAR(m_iDeflected, int, "CTFBaseRocket", "m_iDeflected");
	NETVAR(m_hLauncher, EHANDLE, "CTFBaseRocket", "m_hLauncher");
};

class C_TFProjectile_Flare : public C_TFBaseRocket
{
public:
	NETVAR(m_bCritical, bool, "CTFProjectile_Flare", "m_bCritical");
};

class C_TFProjectile_Rocket : public C_TFBaseRocket
{
public:
	NETVAR(m_bCritical, bool, "CTFProjectile_Rocket", "m_bCritical");
};

class C_TFWeaponBaseGrenadeProj : public C_BaseGrenade
{
public:
	NETVAR(m_vInitialVelocity, Vec3, "CTFWeaponBaseGrenadeProj", "m_vInitialVelocity");
	NETVAR(m_bCritical, bool, "CTFWeaponBaseGrenadeProj", "m_bCritical");
	NETVAR(m_iDeflected, int, "CTFWeaponBaseGrenadeProj", "m_iDeflected");
	NETVAR(m_vecOrigin, Vec3, "CTFWeaponBaseGrenadeProj", "m_vecOrigin");
	NETVAR(m_angRotation, Vec3, "CTFWeaponBaseGrenadeProj", "m_angRotation");
	NETVAR(m_hDeflectOwner, EHANDLE, "CTFWeaponBaseGrenadeProj", "m_hDeflectOwner");
};

class C_TFGrenadePipebombProjectile : public C_TFWeaponBaseGrenadeProj
{
public:
	NETVAR(m_bTouched, bool, "CTFGrenadePipebombProjectile", "m_bTouched");
	NETVAR(m_iType, int, "CTFGrenadePipebombProjectile", "m_iType");
	NETVAR(m_hLauncher, EHANDLE, "CTFGrenadePipebombProjectile", "m_hLauncher");
	NETVAR(m_bDefensiveBomb, int, "CTFGrenadePipebombProjectile", "m_bDefensiveBomb");

	bool HasStickyEffects() { return m_iType() == TF_GL_MODE_REMOTE_DETONATE || m_iType() == TF_GL_MODE_REMOTE_DETONATE_PRACTICE; }

	float m_flCreationTime()
	{
		static int nOffset = NetVars::GetNetVar("CTFGrenadePipebombProjectile", "m_iType") + 4;
		return *reinterpret_cast<float *>(reinterpret_cast<DWORD>(this) + nOffset);
	}
};

class C_TFProjectile_Arrow : public C_TFBaseRocket
{
public:
	NETVAR(m_bArrowAlight, bool, "CTFProjectile_Arrow", "m_bArrowAlight");
	NETVAR(m_bCritical, bool, "CTFProjectile_Arrow", "m_bCritical");
	NETVAR(m_iProjectileType, int, "CTFProjectile_Arrow", "m_iProjectileType");
};