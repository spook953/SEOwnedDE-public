#pragma once
#include "../../Utils/Vector/Vector.h"
#include "basehandle.h"

class CGameTrace;
class IPhysicsSurfaceProps;
enum PLAYER_ANIM;

enum soundlevel_t
{
	SNDLVL_NONE = 0,
	SNDLVL_20dB = 20,
	SNDLVL_25dB = 25,
	SNDLVL_30dB = 30,
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45,
	SNDLVL_50dB = 50,
	SNDLVL_55dB = 55,
	SNDLVL_IDLE = 60,
	SNDLVL_60dB = 60,
	SNDLVL_65dB = 65,
	SNDLVL_STATIC = 66,
	SNDLVL_70dB = 70,
	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75,
	SNDLVL_80dB = 80,
	SNDLVL_TALKING = 80,
	SNDLVL_85dB = 85,
	SNDLVL_90dB = 90,
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100,
	SNDLVL_105dB = 105,
	SNDLVL_110dB = 110,
	SNDLVL_120dB = 120,
	SNDLVL_130dB = 130,
	SNDLVL_GUNFIRE = 140,
	SNDLVL_140dB = 140,
	SNDLVL_150dB = 150,
	SNDLVL_180dB = 180
};

enum
{
	WL_NotInWater = 0,
	WL_Feet,
	WL_Waist,
	WL_Eyes
};

typedef CBaseHandle EntityHandle_t;

#define INVALID_ENTITY_HANDLE INVALID_EHANDLE_INDEX

class IMoveHelper
{
public:
	virtual	char const *GetName(EntityHandle_t handle) const = 0;
	virtual void ResetTouchList(void) = 0;
	virtual bool AddToTouched(const CGameTrace &tr, const Vector &impactvelocity) = 0;
	virtual void ProcessImpacts(void) = 0;
	virtual void Con_NPrintf(int idx, char const *fmt, ...) = 0;
	virtual void StartSound(const Vector &origin, int channel, char const *sample, float volume, soundlevel_t soundlevel, int fFlags, int pitch) = 0;
	virtual void StartSound(const Vector &origin, const char *soundname) = 0;
	virtual void PlaybackEventFull(int flags, int clientindex, unsigned short eventindex, float delay, Vector &origin, Vector &angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) = 0;
	virtual bool PlayerFallingDamage(void) = 0;
	virtual void PlayerSetAnimation(PLAYER_ANIM playerAnim) = 0;
	virtual IPhysicsSurfaceProps *GetSurfaceProps(void) = 0;
	virtual bool IsWorldEntity(const CBaseHandle &handle) = 0;

protected:
	virtual ~IMoveHelper() {}
};

namespace I { inline IMoveHelper *MoveHelper; } //TODO: sig if possible