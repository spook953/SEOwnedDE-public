#pragma once
#include "c_basecombatweapon.h"
#include "c_baseplayer.h"

class CHudTexture
{
public:
	void *vftp;
	char szShortName[64];
	char szTextureFile[64];
	bool bRenderUsingFont;
	bool bPrecached;
	char cCharacterInFont;
	unsigned long hFont;
	int textureId;
	float texCoords[4];
	Rect_t rc;
};

//credits: KGB (all weapon info stuff below)
typedef unsigned short WEAPON_FILE_INFO_HANDLE;

typedef enum
{
	EMPTY,
	SINGLE,
	SINGLE_NPC,
	WPN_DOUBLE,
	DOUBLE_NPC,
	BURST,
	RELOAD,
	RELOAD_NPC,
	MELEE_MISS,
	MELEE_HIT,
	MELEE_HIT_WORLD,
	SPECIAL1,
	SPECIAL2,
	SPECIAL3,
	TAUNT,
	DEPLOY,
	NUM_SHOOT_SOUND_TYPES,
} WeaponSound_t;

#define MAX_SHOOT_SOUNDS 16
#define MAX_WEAPON_STRING 80
#define MAX_WEAPON_PREFIX 16
#define MAX_WEAPON_AMMO_NAME 32

__declspec(align(4)) class FileWeaponInfo_t
{
public:
	void *m_pVtable;

	bool bParsedScript;
	bool bLoadedHudElements;

	// SHARED
	char szClassName[MAX_WEAPON_STRING];
	char szPrintName[MAX_WEAPON_STRING]; // Name for showing in HUD, etc.

	char szViewModel[MAX_WEAPON_STRING];       // View model of this weapon
	char szWorldModel[MAX_WEAPON_STRING];      // Model of this weapon seen carried by the player
	char szAnimationPrefix[MAX_WEAPON_PREFIX]; // Prefix of the animations that should be used by the player carrying this weapon
	int  iSlot;                                // inventory slot.
	int  iPosition;                            // position in the inventory slot.
	int  iMaxClip1;                            // max primary clip size (-1 if no clip)
	int  iMaxClip2;                            // max secondary clip size (-1 if no clip)
	int  iDefaultClip1;                        // amount of primary ammo in the gun when it's created
	int  iDefaultClip2;                        // amount of secondary ammo in the gun when it's created
	int  iWeight;                              // this value used to determine this weapon's importance in autoselection.
	int  iRumbleEffect;                        // Which rumble effect to use when fired? (xbox)
	bool bAutoSwitchTo;                        // whether this weapon should be considered for autoswitching to
	bool bAutoSwitchFrom;                      // whether this weapon can be autoswitched away from when picking up another weapon or ammo
	int  iFlags;                               // miscellaneous weapon flags
	char szAmmo1[MAX_WEAPON_AMMO_NAME];        // "primary" ammo type
	char szAmmo2[MAX_WEAPON_AMMO_NAME];        // "secondary" ammo type

	// Sound blocks
	char aShootSounds[NUM_SHOOT_SOUND_TYPES][MAX_WEAPON_STRING];

	int  iAmmoType;
	int  iAmmo2Type;
	bool m_bMeleeWeapon; // Melee weapons can always "fire" regardless of ammo.

	// This tells if the weapon was built right-handed (defaults to true).
	// This helps cl_righthand make the decision about whether to flip the model or not.
	bool m_bBuiltRightHanded;
	bool m_bAllowFlipping; // False to disallow flipping the model, regardless of whether
	// it is built left or right handed.

	// CLIENT DLL
	// Sprite data, read from the data file
	int   iSpriteCount;
	CHudTexture *iconActive;
	CHudTexture *iconInactive;
	CHudTexture *iconAmmo;
	CHudTexture *iconAmmo2;
	CHudTexture *iconCrosshair;
	CHudTexture *iconAutoaim;
	CHudTexture *iconZoomedCrosshair;
	CHudTexture *iconZoomedAutoaim;
	CHudTexture *iconSmall;

	// TF2 specific
	bool bShowUsageHint; // if true, then when you receive the weapon, show a hint about it
};

__declspec(align(4)) struct WeaponData_t
{
	int   m_nDamage;
	int   m_nBulletsPerShot;
	float m_flRange;
	float m_flSpread;
	float m_flPunchAngle;
	float m_flTimeFireDelay;   // Time to delay between firing
	float m_flTimeIdle;        // Time to idle after firing
	float m_flTimeIdleEmpty;   // Time to idle after firing last bullet in clip
	float m_flTimeReloadStart; // Time to start into a reload (ie. shotgun)
	float m_flTimeReload;      // Time to reload
	bool  m_bDrawCrosshair;    // Should the weapon draw a crosshair
	int   m_iProjectile;       // The type of projectile this mode fires
	int   m_iAmmoPerShot;      // How much ammo each shot consumes
	float m_flProjectileSpeed; // Start speed for projectiles (nail, etc.); NOTE: union with something non-projectile
	float m_flSmackDelay;      // how long after swing should damage happen for melee weapons
	bool  m_bUseRapidFireCrits;

	void Init()
	{
		m_nDamage = 0;
		m_nBulletsPerShot = 0;
		m_flRange = 0.0f;
		m_flSpread = 0.0f;
		m_flPunchAngle = 0.0f;
		m_flTimeFireDelay = 0.0f;
		m_flTimeIdle = 0.0f;
		m_flTimeIdleEmpty = 0.0f;
		m_flTimeReloadStart = 0.0f;
		m_flTimeReload = 0.0f;
		m_iProjectile = 0;
		m_iAmmoPerShot = 0;
		m_flProjectileSpeed = 0.0f;
		m_flSmackDelay = 0.0f;
		m_bUseRapidFireCrits = false;
	};
};

__declspec(align(4)) class CTFWeaponInfo : public FileWeaponInfo_t
{
public:
	WeaponData_t m_WeaponData[2];                    //0x06F8
	int          m_iWeaponType;                      //0x0778
	int          m_bGrenade;                         //0x077C
	float        m_flDamageRadius;                   //0x0780
	float        m_flPrimerTime;                     //0x0784
	bool         m_bLowerWeapon;                     //0x0788
	bool         m_bSuppressGrenTimer;               //0x0789
	bool         m_bHasTeamSkins_Viewmodel;          //0x078A
	bool         m_bHasTeamSkins_Worldmodel;         //0x078B
	char         m_szMuzzleFlashModel[128];          //0x078C
	float        m_flMuzzleFlashModelDuration;       //0x080C
	char         m_szMuzzleFlashParticleEffect[128]; //0x0810
	char         m_szTracerEffect[128];              //0x0890
	bool         m_bDoInstantEjectBrass;             //0x0910
	char         m_szBrassModel[128];                //0x0911
	char         m_szExplosionSound[128];            //0x0991
	char         m_szExplosionEffect[128];           //0x0A11
	char         m_szExplosionPlayerEffect[128];     //0x0A91
	char         m_szExplosionWaterEffect[128];      //0x0B11
	bool         m_bDontDrop;                        //0x0B91

	WeaponData_t const &GetWeaponData(int iWeapon) const { return m_WeaponData[iWeapon]; }
};

class C_TFWeaponBase : public C_BaseCombatWeapon
{
public:
	NETVAR(m_bLowered, bool, "CTFWeaponBase", "m_bLowered");
	NETVAR(m_iReloadMode, int, "CTFWeaponBase", "m_iReloadMode");
	NETVAR(m_bResetParity, bool, "CTFWeaponBase", "m_bResetParity");
	NETVAR(m_bReloadedThroughAnimEvent, bool, "CTFWeaponBase", "m_bReloadedThroughAnimEvent");
	NETVAR(m_bDisguiseWeapon, bool, "CTFWeaponBase", "m_bDisguiseWeapon");
	NETVAR(m_flLastCritCheckTime, float, "CTFWeaponBase", "m_flLastCritCheckTime");
	NETVAR(m_flReloadPriorNextFire, float, "CTFWeaponBase", "m_flReloadPriorNextFire");
	NETVAR(m_flLastFireTime, float, "CTFWeaponBase", "m_flLastFireTime");
	NETVAR(m_flEffectBarRegenTime, float, "CTFWeaponBase", "m_flEffectBarRegenTime");
	NETVAR(m_flObservedCritChance, float, "CTFWeaponBase", "m_flObservedCritChance");
	NETVAR(m_flEnergy, float, "CTFWeaponBase", "m_flEnergy");
	NETVAR(m_hExtraWearable, EHANDLE, "CTFWeaponBase", "m_hExtraWearable");
	NETVAR(m_hExtraWearableViewModel, EHANDLE, "CTFWeaponBase", "m_hExtraWearableViewModel");
	NETVAR(m_bBeingRepurposedForTaunt, bool, "CTFWeaponBase", "m_bBeingRepurposedForTaunt");
	NETVAR(m_nKillComboClass, int, "CTFWeaponBase", "m_nKillComboClass");
	NETVAR(m_nKillComboCount, int, "CTFWeaponBase", "m_nKillComboCount");
	NETVAR(m_flInspectAnimEndTime, float, "CTFWeaponBase", "m_flInspectAnimEndTime");
	NETVAR(m_nInspectStage, int, "CTFWeaponBase", "m_nInspectStage");
	NETVAR(m_iConsecutiveShots, int, "CTFWeaponBase", "m_iConsecutiveShots");
	NETVAR(m_iItemDefinitionIndex, int, "CEconEntity", "m_iItemDefinitionIndex");

	int GetSlot() {
		return reinterpret_cast<int(__thiscall *)(void *)>(Memory::GetVFunc(this, 330))(this);
	}

	int GetWeaponID() {
		return reinterpret_cast<int(__thiscall *)(void *)>(Memory::GetVFunc(this, 381))(this);
	}

	int GetDamageType() {
		return reinterpret_cast<int(__thiscall *)(void *)>(Memory::GetVFunc(this, 382))(this);
	}

	float GetSwingRange() {
		return GetWeaponID() == TF_WEAPON_SWORD ? 72.0f : 48.0f;
	}

	float &m_flSmackTime() {
		static int nOffset = NetVars::GetNetVar("CTFWeaponBase", "m_nInspectStage") + 0x1C;
		return *reinterpret_cast<float *>(reinterpret_cast<DWORD>(this) + nOffset);
	}

	bool CanFireCriticalShot(C_BasePlayer *pOwner, bool bIsHeadshot = false)
	{
		bool bOut = false;

		if (pOwner)
		{
			int &iFOV = pOwner->m_iFOV(), nFovBackup = iFOV;
			iFOV = 70;
			bOut = reinterpret_cast<bool(__thiscall *)(void *, bool, void *)>(Memory::GetVFunc(this, 425))(this, bIsHeadshot, nullptr);
			iFOV = nFovBackup;
		}

		return bOut;
	}

	bool CanHeadShot(C_BasePlayer *pOwner)
	{
		bool bOut = false;

		if (pOwner)
			bOut = (GetDamageType() & DMG_USE_HITLOCATIONS) && CanFireCriticalShot(pOwner, true);

		return bOut;
	}

	bool CanPrimaryAttack(C_BasePlayer *pOwner)
	{
		bool bOut = false;

		if (pOwner) {
			float flCurTime = static_cast<float>(pOwner->m_nTickBase()) * I::GlobalVars->interval_per_tick;
			bOut = m_flNextPrimaryAttack() <= flCurTime && pOwner->m_flNextAttack() <= flCurTime;
		}

		return bOut;
	}

	bool CanSecondaryAttack(C_BasePlayer *pOwner)
	{
		bool bOut = false;

		if (pOwner) {
			float flCurTime = static_cast<float>(pOwner->m_nTickBase()) * I::GlobalVars->interval_per_tick;
			bOut = m_flNextSecondaryAttack() <= flCurTime && pOwner->m_flNextAttack() <= flCurTime;
		}

		return bOut;
	}

	CTFWeaponInfo *GetWeaponInfo() {
		static int nOffset = NetVars::GetNetVar("CTFWeaponBase", "m_flEffectBarRegenTime") + 0x10;
		return *reinterpret_cast<CTFWeaponInfo **>(reinterpret_cast<DWORD>(this) + nOffset);
	}
	
	bool IsEnergyWeapon() {
		return reinterpret_cast<bool(__thiscall *)(void *)>(Memory::GetVFunc(this, 432))(this);
	}

	bool HasPrimaryAmmoForShot()
	{
		if (IsEnergyWeapon())
			return m_flEnergy() > 0.0f;

		int nClip1 = m_iClip1();

		if (nClip1 == -1)
		{
			if (auto pOwner = m_hOwnerEntity().Get())
			{
				int nAmmoCount = pOwner->As<C_BasePlayer>()->GetAmmoCount(m_iPrimaryAmmoType());

				if (m_iItemDefinitionIndex() == Engi_m_TheWidowmaker)
					return nAmmoCount > 29;
				
				return nAmmoCount > 0;
			}
		}

		return nClip1 > 0;
	}

	int &m_iCurrentSeed() {
		return *reinterpret_cast<int *>(reinterpret_cast<DWORD>(this) + 0xB5C);
	}

	void GetProjectileFireSetup(void *pPlayer, Vector vecOffset, Vector *vecSrc, QAngle *angForward, bool bHitTeammates = true, float flEndDist = 2000.0f) {
		using fn = void(__thiscall *)(C_TFWeaponBase *, void *, Vector, Vector *, QAngle *, bool, float);
		reinterpret_cast<fn>(Memory::GetVFunc(this, 399))(this, pPlayer, vecOffset, vecSrc, angForward, bHitTeammates, flEndDist);
	}

	void GetSpreadAngles(Vec3 &out) {
		reinterpret_cast<void(__thiscall *)(void *, Vec3 &)>(Signatures::CTFWeaponBase_GetSpreadAngles.Get())(this, out);
	}

	void UpdateAllViewmodelAddons() {
		return reinterpret_cast<void(__thiscall *)(void *)>(Signatures::CTFWeaponBase_UpdateAllViewmodelAddons.Get())(this);
	}

	float ApplyFireDelay(float flDelay) {
		return reinterpret_cast<float(__thiscall *)(void *, float)>(Memory::GetVFunc(this, 407))(this, flDelay);
	}

	bool CalcIsAttackCriticalHelperMelee() {
		return reinterpret_cast<bool(__thiscall *)(void *)>(Signatures::CTFWeaponBaseMelee_CalcIsAttackCriticalHelper.Get())(this);
	}

	bool CalcIsAttackCriticalHelper() {
		return reinterpret_cast<bool(__thiscall *)(void *)>(Signatures::C_TFWeaponBase_CalcIsAttackCriticalHelper.Get())(this);
	}

	C_BaseAnimating *GetAppropriateWorldOrViewModel() {
		return reinterpret_cast<C_BaseAnimating * (__thiscall *)(void *)>(Signatures::C_TFWeaponBase_GetAppropriateWorldOrViewModel.Get())(this);
	}

	float GetWeaponSpread()
	{
		return reinterpret_cast<float(__thiscall *)(void *)>(Signatures::C_TFWeaponBaseGun_GetWeaponSpread.Get())(this);
	}
};

class C_TFKnife : public C_TFWeaponBase
{
public:
	NETVAR(m_bReadyToBackstab, bool, "CTFKnife", "m_bReadyToBackstab");
	NETVAR(m_bKnifeExists, bool, "CTFKnife", "m_bKnifeExists");
	NETVAR(m_flKnifeRegenerateDuration, float, "CTFKnife", "m_flKnifeRegenerateDuration");
	NETVAR(m_flKnifeMeltTimestamp, float, "CTFKnife", "m_flKnifeMeltTimestamp");
};

class C_TFMinigun : public C_TFWeaponBase
{
public:
	NETVAR(m_iWeaponState, int, "CTFMinigun", "m_iWeaponState");
	NETVAR(m_bCritShot, bool, "CTFMinigun", "m_bCritShot");
};

class C_WeaponMedigun : public C_TFWeaponBase
{
public:
	NETVAR(m_hHealingTarget, EHANDLE, "CWeaponMedigun", "m_hHealingTarget");
	NETVAR(m_bHealing, bool, "CWeaponMedigun", "m_bHealing");
	NETVAR(m_bAttacking, bool, "CWeaponMedigun", "m_bAttacking");
	NETVAR(m_bChargeRelease, bool, "CWeaponMedigun", "m_bChargeRelease");
	NETVAR(m_bHolstered, bool, "CWeaponMedigun", "m_bHolstered");
	NETVAR(m_nChargeResistType, int, "CWeaponMedigun", "m_nChargeResistType");
	NETVAR(m_hLastHealingTarget, EHANDLE, "CWeaponMedigun", "m_hLastHealingTarget");
	NETVAR(m_flChargeLevel, float, "CWeaponMedigun", "m_flChargeLevel");

	int GetMedigunType()
	{
		int iMode = 0;
		iMode = static_cast<int>(SDKUtils::AttribHookValue(static_cast<float>(iMode), "set_weapon_mode", this));
		return iMode;
	}

	medigun_charge_types GetChargeType()
	{
		int iTmp = MEDIGUN_CHARGE_INVULN;
		iTmp = static_cast<int>(SDKUtils::AttribHookValue(static_cast<float>(iTmp), "set_charge_type", this));

		if (GetMedigunType() == MEDIGUN_RESIST)
			iTmp += m_nChargeResistType();

		return (medigun_charge_types)iTmp;
	}

	medigun_resist_types_t GetResistType()
	{
		int nCurrentActiveResist = (GetChargeType() - MEDIGUN_CHARGE_BULLET_RESIST);
		nCurrentActiveResist = nCurrentActiveResist % MEDIGUN_NUM_RESISTS;
		return medigun_resist_types_t(nCurrentActiveResist);
	}
};

class C_TFPipebombLauncher : public C_TFWeaponBase
{
public:
	NETVAR(m_iPipebombCount, int, "CTFPipebombLauncher", "m_iPipebombCount");
	NETVAR(m_flChargeBeginTime, float, "CTFPipebombLauncher", "m_flChargeBeginTime");

	CUtlVector<CHandle<C_TFGrenadePipebombProjectile>> &m_Pipebombs()
	{
		static int nOffset = NetVars::GetNetVar("CTFPipebombLauncher", "m_flChargeBeginTime") - 28;
		return *reinterpret_cast<CUtlVector<CHandle<C_TFGrenadePipebombProjectile>> *>(reinterpret_cast<DWORD>(this) + nOffset);
	}
};

class C_TFSniperRifle : public C_TFWeaponBase
{
public:
	NETVAR(m_flChargedDamage, float, "CTFSniperRifle", "m_flChargedDamage");
};

class C_TFGrenadeLauncher : public C_TFWeaponBase
{
public:
	NETVAR(m_flDetonateTime, float, "CTFGrenadeLauncher", "m_flDetonateTime");
	NETVAR(m_iCurrentTube, int, "CTFGrenadeLauncher", "m_iCurrentTube");
	NETVAR(m_iGoalTube, int, "CTFGrenadeLauncher", "m_iGoalTube");
};

class C_TFSniperRifleClassic : public C_TFSniperRifle
{
public:
	NETVAR(m_bCharging, bool, "CTFSniperRifleClassic", "m_bCharging");
};