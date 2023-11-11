#pragma once
#include "../../Utils/Vector/Vector.h"

#define TICK_INTERVAL I::GlobalVars->interval_per_tick
#define TIME_TO_TICKS(dt) (static_cast<int>(0.5f + static_cast<float>(dt) / TICK_INTERVAL))
#define TICKS_TO_TIME(t) (TICK_INTERVAL * t)

#define ANIMATION_CYCLE_BITS 10
#define ANIMATION_CYCLE_MINFRAC (1.0f / (1<<ANIMATION_CYCLE_BITS))

class CViewVectors
{
public:
	Vector m_vView;
	Vector m_vHullMin;
	Vector m_vHullMax;
	Vector m_vDuckHullMin;
	Vector m_vDuckHullMax;
	Vector m_vDuckView;
	Vector m_vObsHullMin;
	Vector m_vObsHullMax;
	Vector m_vDeadViewHeight;
};

//TODO: add game rules
//#define VEC_VIEW g_pGameRules->GetViewVectors()->m_vView
//#define VEC_HULL_MIN g_pGameRules->GetViewVectors()->m_vHullMin
//#define VEC_HULL_MAX g_pGameRules->GetViewVectors()->m_vHullMax
//#define VEC_DUCK_HULL_MIN g_pGameRules->GetViewVectors()->m_vDuckHullMin
//#define VEC_DUCK_HULL_MAX g_pGameRules->GetViewVectors()->m_vDuckHullMax
//#define VEC_DUCK_VIEW g_pGameRules->GetViewVectors()->m_vDuckView
//#define VEC_OBS_HULL_MIN g_pGameRules->GetViewVectors()->m_vObsHullMin
//#define VEC_OBS_HULL_MAX g_pGameRules->GetViewVectors()->m_vObsHullMax
//#define VEC_DEAD_VIEWHEIGHT	g_pGameRules->GetViewVectors()->m_vDeadViewHeight
//
//#define VEC_VIEW_SCALED( player ) ( g_pGameRules->GetViewVectors()->m_vView * player->GetModelScale() )
//#define VEC_HULL_MIN_SCALED( player ) ( g_pGameRules->GetViewVectors()->m_vHullMin * player->GetModelScale() )
//#define VEC_HULL_MAX_SCALED( player ) ( g_pGameRules->GetViewVectors()->m_vHullMax * player->GetModelScale() )
//#define VEC_DUCK_HULL_MIN_SCALED( player ) ( g_pGameRules->GetViewVectors()->m_vDuckHullMin * player->GetModelScale() )
//#define VEC_DUCK_HULL_MAX_SCALED( player ) ( g_pGameRules->GetViewVectors()->m_vDuckHullMax * player->GetModelScale() )
//#define VEC_DUCK_VIEW_SCALED( player ) ( g_pGameRules->GetViewVectors()->m_vDuckView * player->GetModelScale() )
//#define VEC_OBS_HULL_MIN_SCALED( player ) ( g_pGameRules->GetViewVectors()->m_vObsHullMin * player->GetModelScale() )
//#define VEC_OBS_HULL_MAX_SCALED( player ) ( g_pGameRules->GetViewVectors()->m_vObsHullMax * player->GetModelScale() )
//#define VEC_DEAD_VIEWHEIGHT_SCALED( player ) ( g_pGameRules->GetViewVectors()->m_vDeadViewHeight * player->GetModelScale() )

#define WATERJUMP_HEIGHT			8

#define MAX_CLIMB_SPEED		200

#if defined(TF_DLL) || defined(TF_CLIENT_DLL)
#define TIME_TO_DUCK		0.2
#define TIME_TO_DUCK_MS		200.0f
#else
#define TIME_TO_DUCK 0.4f
#define TIME_TO_DUCK_MS	400.0f
#endif 
#define TIME_TO_UNDUCK 0.2f
#define TIME_TO_UNDUCK_MS 200.0f

#define MAX_WEAPON_SLOTS 6
#define MAX_WEAPON_POSITIONS 20
#define MAX_ITEM_TYPES 6
#define MAX_WEAPONS 48

#define MAX_ITEMS 5

#define WEAPON_NOCLIP -1

#define	MAX_AMMO_TYPES 32
#define MAX_AMMO_SLOTS 32

#define HUD_PRINTNOTIFY 1
#define HUD_PRINTCONSOLE 2
#define HUD_PRINTTALK 3
#define HUD_PRINTCENTER 4

typedef enum
{
	VOTE_FAILED_GENERIC = 0,
	VOTE_FAILED_TRANSITIONING_PLAYERS,
	VOTE_FAILED_RATE_EXCEEDED,
	VOTE_FAILED_YES_MUST_EXCEED_NO,
	VOTE_FAILED_QUORUM_FAILURE,
	VOTE_FAILED_ISSUE_DISABLED,
	VOTE_FAILED_MAP_NOT_FOUND,
	VOTE_FAILED_MAP_NAME_REQUIRED,
	VOTE_FAILED_ON_COOLDOWN,
	VOTE_FAILED_TEAM_CANT_CALL,
	VOTE_FAILED_WAITINGFORPLAYERS,
	VOTE_FAILED_PLAYERNOTFOUND,
	VOTE_FAILED_CANNOT_KICK_ADMIN,
	VOTE_FAILED_SCRAMBLE_IN_PROGRESS,
	VOTE_FAILED_SPECTATOR,
	VOTE_FAILED_NEXTLEVEL_SET,
	VOTE_FAILED_MAP_NOT_VALID,
	VOTE_FAILED_CANNOT_KICK_FOR_TIME,
	VOTE_FAILED_CANNOT_KICK_DURING_ROUND,
	VOTE_FAILED_VOTE_IN_PROGRESS,
	VOTE_FAILED_KICK_LIMIT_REACHED,
	VOTE_FAILED_KICK_DENIED_BY_GC,
	VOTE_FAILED_MODIFICATION_ALREADY_ACTIVE
} vote_create_failed_t;

enum
{
	SERVER_MODIFICATION_ITEM_DURATION_IN_MINUTES = 120
};

#define MAX_VOTE_DETAILS_LENGTH 64
#define INVALID_ISSUE -1
#define MAX_VOTE_OPTIONS 5
#define DEDICATED_SERVER 99

enum CastVote
{
	VOTE_OPTION1,
	VOTE_OPTION2,
	VOTE_OPTION3,
	VOTE_OPTION4,
	VOTE_OPTION5,
	VOTE_UNCAST
};

#define TEAM_ANY -2
#define	TEAM_INVALID -1
#define TEAM_UNASSIGNED 0
#define TEAM_SPECTATOR 1
#define LAST_SHARED_TEAM TEAM_SPECTATOR
#define FIRST_GAME_TEAM (LAST_SHARED_TEAM+1)
#define MAX_TEAMS 32
#define MAX_TEAM_NAME_LENGTH 32

#define TRACER_FLAG_WHIZ 0x0001
#define TRACER_FLAG_USEATTACHMENT 0x0002
#define TRACER_DONT_USE_ATTACHMENT	-1

enum
{
	ENTITY_DISSOLVE_NORMAL = 0,
	ENTITY_DISSOLVE_ELECTRICAL,
	ENTITY_DISSOLVE_ELECTRICAL_LIGHT,
	ENTITY_DISSOLVE_CORE,
	ENTITY_DISSOLVE_BITS = 3
};

#define	HITGROUP_GENERIC 0
#define	HITGROUP_HEAD 1
#define	HITGROUP_CHEST 2
#define	HITGROUP_STOMACH 3
#define HITGROUP_LEFTARM 4	
#define HITGROUP_RIGHTARM 5
#define HITGROUP_LEFTLEG 6
#define HITGROUP_RIGHTLEG 7
#define HITGROUP_GEAR 10

#define DMG_GENERIC 0		
#define DMG_CRUSH (1 << 0)
#define DMG_BULLET (1 << 1)
#define DMG_SLASH (1 << 2)
#define DMG_BURN (1 << 3)
#define DMG_VEHICLE (1 << 4)
#define DMG_FALL (1 << 5)
#define DMG_BLAST (1 << 6)
#define DMG_CLUB (1 << 7)
#define DMG_SHOCK (1 << 8)
#define DMG_SONIC (1 << 9)
#define DMG_ENERGYBEAM (1 << 10)
#define DMG_PREVENT_PHYSICS_FORCE (1 << 11) 
#define DMG_NEVERGIB (1 << 12)
#define DMG_ALWAYSGIB (1 << 13)
#define DMG_DROWN (1 << 14)


#define DMG_PARALYZE (1 << 15)
#define DMG_NERVEGAS (1 << 16)
#define DMG_POISON (1 << 17)
#define DMG_RADIATION (1 << 18)
#define DMG_DROWNRECOVER (1 << 19)
#define DMG_ACID (1 << 20)
#define DMG_SLOWBURN (1 << 21)

#define DMG_REMOVENORAGDOLL	(1<<22)
									
#define DMG_PHYSGUN (1<<23)
#define DMG_PLASMA (1<<24)
#define DMG_AIRBOAT (1<<25)

#define DMG_DISSOLVE (1<<26)
#define DMG_BLAST_SURFACE (1<<27)
#define DMG_DIRECT (1<<28)
#define DMG_BUCKSHOT (1<<29)

#define DMG_LASTGENERICFLAG	DMG_BUCKSHOT

#define	DAMAGE_NO 0
#define DAMAGE_EVENTS_ONLY 1
#define	DAMAGE_YES 2
#define	DAMAGE_AIM 3

enum
{
	OBS_MODE_NONE = 0,
	OBS_MODE_DEATHCAM,
	OBS_MODE_FREEZECAM,
	OBS_MODE_FIXED,
	OBS_MODE_IN_EYE,
	OBS_MODE_CHASE,
	OBS_MODE_POI,
	OBS_MODE_ROAMING,
	NUM_OBSERVER_MODES
};

#define LAST_PLAYER_OBSERVERMODE OBS_MODE_ROAMING

enum
{
	OBS_ALLOW_ALL = 0,
	OBS_ALLOW_TEAM,
	OBS_ALLOW_NONE,
	OBS_ALLOW_NUM_MODES
};

enum
{
	TYPE_TEXT = 0,
	TYPE_INDEX,
	TYPE_URL,
	TYPE_FILE
};

enum
{
	EFL_KILLME = (1 << 0),
	EFL_DORMANT = (1 << 1),
	EFL_NOCLIP_ACTIVE = (1 << 2),
	EFL_SETTING_UP_BONES = (1 << 3),
	EFL_KEEP_ON_RECREATE_ENTITIES = (1 << 4),
	EFL_HAS_PLAYER_CHILD = (1 << 4),
	EFL_DIRTY_SHADOWUPDATE = (1 << 5),
	EFL_NOTIFY = (1 << 6),
	EFL_FORCE_CHECK_TRANSMIT = (1 << 7),
	EFL_BOT_FROZEN = (1 << 8),
	EFL_SERVER_ONLY = (1 << 9),
	EFL_NO_AUTO_EDICT_ATTACH = (1 << 10),
	EFL_DIRTY_ABSTRANSFORM = (1 << 11),
	EFL_DIRTY_ABSVELOCITY = (1 << 12),
	EFL_DIRTY_ABSANGVELOCITY = (1 << 13),
	EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = (1 << 14),
	EFL_DIRTY_SPATIAL_PARTITION = (1 << 15),
	EFL_IN_SKYBOX = (1 << 17),
	EFL_USE_PARTITION_WHEN_NOT_SOLID = (1 << 18),
	EFL_TOUCHING_FLUID = (1 << 19),
	EFL_IS_BEING_LIFTED_BY_BARNACLE = (1 << 20),
	EFL_NO_ROTORWASH_PUSH = (1 << 21),
	EFL_NO_THINK_FUNCTION = (1 << 22),
	EFL_NO_GAME_PHYSICS_SIMULATION = (1 << 23),
	EFL_CHECK_UNTOUCH = (1 << 24),
	EFL_DONTBLOCKLOS = (1 << 25),
	EFL_DONTWALKON = (1 << 26),
	EFL_NO_DISSOLVE = (1 << 27),
	EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1 << 28),
	EFL_NO_WATER_VELOCITY_CHANGE = (1 << 29),
	EFL_NO_PHYSCANNON_INTERACTION = (1 << 30),
	EFL_NO_DAMAGE_FORCES = (1 << 31)
};

class CBaseEntity;

struct FireBulletsInfo_t
{
	int m_iShots;
	Vector m_vecSrc;
	Vector m_vecDirShooting;
	Vector m_vecSpread;
	float m_flDistance;
	int m_iAmmoType;
	int m_iTracerFreq;
	float m_flDamage;
	int m_iPlayerDamage;
	int m_nFlags;
	float m_flDamageForceScale;
	CBaseEntity *m_pAttacker;
	CBaseEntity *m_pAdditionalIgnoreEnt;
	bool m_bPrimaryAttack;
	bool m_bUseServerRandomSeed;
};

#define TF_VISION_FILTER_NONE 0
#define TF_VISION_FILTER_PYRO (1<<0)
#define TF_VISION_FILTER_HALLOWEEN (1<<1)
#define TF_VISION_FILTER_ROME (1<<2)

enum
{
	VISION_MODE_NONE = 0,
	VISION_MODE_PYRO,
	VISION_MODE_HALLOWEEN,
	VISION_MODE_ROME,
	MAX_VISION_MODES
};