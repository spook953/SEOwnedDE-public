#pragma once
#include "c_basecombatcharacter.h"
#include "usercmd.h"
#include "Signatures.h"

class C_BasePlayer : public C_BaseCombatCharacter
{
public:
	NETVAR(m_Local, void *, "CBasePlayer", "m_Local");
	NETVAR(m_chAreaBits, void *, "CBasePlayer", "m_chAreaBits");
	NETVAR(m_chAreaPortalBits, void *, "CBasePlayer", "m_chAreaPortalBits");
	NETVAR(m_iHideHUD, int, "CBasePlayer", "m_iHideHUD");
	NETVAR(m_flFOVRate, float, "CBasePlayer", "m_flFOVRate");
	NETVAR(m_bDucked, bool, "CBasePlayer", "m_bDucked");
	NETVAR(m_bDucking, bool, "CBasePlayer", "m_bDucking");
	NETVAR(m_bInDuckJump, bool, "CBasePlayer", "m_bInDuckJump");
	NETVAR(m_flDucktime, float, "CBasePlayer", "m_flDucktime");
	NETVAR(m_flDuckJumpTime, float, "CBasePlayer", "m_flDuckJumpTime");
	NETVAR(m_flJumpTime, float, "CBasePlayer", "m_flJumpTime");
	NETVAR(m_flFallVelocity, float, "CBasePlayer", "m_flFallVelocity");
	NETVAR(m_vecPunchAngle, Vec3, "CBasePlayer", "m_vecPunchAngle");
	NETVAR(m_vecPunchAngleVel, Vec3, "CBasePlayer", "m_vecPunchAngleVel");
	NETVAR(m_bDrawViewmodel, bool, "CBasePlayer", "m_bDrawViewmodel");
	NETVAR(m_bWearingSuit, bool, "CBasePlayer", "m_bWearingSuit");
	NETVAR(m_bPoisoned, bool, "CBasePlayer", "m_bPoisoned");
	NETVAR(m_flStepSize, float, "CBasePlayer", "m_flStepSize");
	NETVAR(m_bAllowAutoMovement, bool, "CBasePlayer", "m_bAllowAutoMovement");
	NETVAR(m_vecViewOffset, Vec3, "CBasePlayer", "m_vecViewOffset[0]");
	NETVAR(m_flFriction, float, "CBasePlayer", "m_flFriction");
	NETVAR(m_iAmmo, void *, "CBasePlayer", "m_iAmmo");
	NETVAR(m_fOnTarget, int, "CBasePlayer", "m_fOnTarget");
	NETVAR(m_nTickBase, int, "CBasePlayer", "m_nTickBase");
	NETVAR(m_nNextThinkTick, int, "CBasePlayer", "m_nNextThinkTick");
	NETVAR(m_hLastWeapon, EHANDLE, "CBasePlayer", "m_hLastWeapon");
	NETVAR(m_hGroundEntity, EHANDLE, "CBasePlayer", "m_hGroundEntity");
	NETVAR(m_vecVelocity, Vec3, "CBasePlayer", "m_vecVelocity[0]");
	NETVAR(m_vecBaseVelocity, Vec3, "CBasePlayer", "m_vecBaseVelocity");
	NETVAR(m_hConstraintEntity, EHANDLE, "CBasePlayer", "m_hConstraintEntity");
	NETVAR(m_vecConstraintCenter, Vec3, "CBasePlayer", "m_vecConstraintCenter");
	NETVAR(m_flConstraintRadius, float, "CBasePlayer", "m_flConstraintRadius");
	NETVAR(m_flConstraintWidth, float, "CBasePlayer", "m_flConstraintWidth");
	NETVAR(m_flConstraintSpeedFactor, float, "CBasePlayer", "m_flConstraintSpeedFactor");
	NETVAR(m_flDeathTime, float, "CBasePlayer", "m_flDeathTime");
	NETVAR(m_nWaterLevel, int, "CBasePlayer", "m_nWaterLevel");
	NETVAR(m_flLaggedMovementValue, float, "CBasePlayer", "m_flLaggedMovementValue");
	NETVAR(m_AttributeList, void *, "CBasePlayer", "m_AttributeList");
	NETVAR(pl, void *, "CBasePlayer", "pl");
	NETVAR(deadflag, int, "CBasePlayer", "deadflag");
	NETVAR(m_iFOV, int, "CBasePlayer", "m_iFOV");
	NETVAR(m_iFOVStart, int, "CBasePlayer", "m_iFOVStart");
	NETVAR(m_flFOVTime, float, "CBasePlayer", "m_flFOVTime");
	NETVAR(m_iDefaultFOV, int, "CBasePlayer", "m_iDefaultFOV");
	NETVAR(m_hZoomOwner, EHANDLE, "CBasePlayer", "m_hZoomOwner");
	NETVAR(m_hVehicle, EHANDLE, "CBasePlayer", "m_hVehicle");
	NETVAR(m_hUseEntity, EHANDLE, "CBasePlayer", "m_hUseEntity");
	NETVAR(m_iHealth, int, "CBasePlayer", "m_iHealth");
	NETVAR(m_lifeState, byte, "CBasePlayer", "m_lifeState");
	NETVAR(m_iBonusProgress, int, "CBasePlayer", "m_iBonusProgress");
	NETVAR(m_iBonusChallenge, int, "CBasePlayer", "m_iBonusChallenge");
	NETVAR(m_flMaxspeed, float, "CBasePlayer", "m_flMaxspeed");
	NETVAR(m_fFlags, int, "CBasePlayer", "m_fFlags");
	NETVAR(m_iObserverMode, int, "CBasePlayer", "m_iObserverMode");
	NETVAR(m_hObserverTarget, EHANDLE, "CBasePlayer", "m_hObserverTarget");
	NETVAR(m_hViewModel, EHANDLE, "CBasePlayer", "m_hViewModel[0]");
	NETVAR(m_szLastPlaceName, const char *, "CBasePlayer", "m_szLastPlaceName");

	float &m_flWaterJumpTime() {
		return *reinterpret_cast<float *>(reinterpret_cast<DWORD>(this) + 0x1120);
	}

	float &m_flSwimSoundTime() {
		return *reinterpret_cast<float *>(reinterpret_cast<DWORD>(this) + 0x1128);
	}

	Vec3 &m_vecLadderNormal() {
		return *reinterpret_cast<Vec3 *>(reinterpret_cast<DWORD>(this) + 0x112C);
	}

	void SetCurrentCommand(CUserCmd *pCmd) {
		static int nOffset = NetVars::GetNetVar("CBasePlayer", "m_hConstraintEntity") - 4;
		*reinterpret_cast<CUserCmd **>(reinterpret_cast<DWORD>(this) + nOffset) = pCmd;
	}

	int &m_nButtons() {
		static int nOffset = NetVars::GetNetVar("CBasePlayer", "m_hConstraintEntity") - 8;
		return *reinterpret_cast<int *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	int &m_surfaceProps() {
		static int nOffset = 4768;
		return *reinterpret_cast<int *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	void *&m_pSurfaceData() {
		static int nOffset = 4772;
		return *reinterpret_cast<void **>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	float &m_surfaceFriction() {
		static int nOffset = 4776;
		return *reinterpret_cast<float *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	char &m_chTextureType() {
		static int nOffset = 4780;
		return *reinterpret_cast<char *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	int GetAmmoCount(int iAmmoType) {
		return reinterpret_cast<int(__thiscall *)(void *, int)>(Signatures::CBasePlayer_GetAmmoCount.Get())(this, iAmmoType);
	}

	int m_afButtonLast() {
		static int nOffset = NetVars::GetNetVar("CBasePlayer", "m_hConstraintEntity") - 20;
		return *reinterpret_cast<int *>(reinterpret_cast<DWORD>(this) + nOffset);
	}
};