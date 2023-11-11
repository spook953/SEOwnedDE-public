#pragma once

#include "interface.h"

#define TE_EXPLFLAG_NONE		0x0			// all flags clear makes default Half-Life explosion
#define TE_EXPLFLAG_NOADDITIVE	0x1			// sprite will be drawn opaque (ensure that the sprite you send is a non-additive sprite)
#define TE_EXPLFLAG_NODLIGHTS	0x2			// do not render dynamic lights
#define TE_EXPLFLAG_NOSOUND		0x4			// do not play client explosion sound
#define TE_EXPLFLAG_NOPARTICLES	0x8			// do not draw particles
#define TE_EXPLFLAG_DRAWALPHA	0x10		// sprite will be drawn alpha
#define TE_EXPLFLAG_ROTATE		0x20		// rotate the sprite randomly
#define TE_EXPLFLAG_NOFIREBALL	0x40		// do not draw a fireball
#define TE_EXPLFLAG_NOFIREBALLSMOKE	0x80	// do not draw smoke with the fireball

#define	TE_BEAMPOINTS		0		// beam effect between two points
#define TE_SPRITE			1		// additive sprite, plays 1 cycle
#define TE_BEAMDISK			2		// disk that expands to max radius over lifetime
#define TE_BEAMCYLINDER		3		// cylinder that expands to max radius over lifetime
#define TE_BEAMFOLLOW		4		// create a line of decaying beam segments until entity stops moving
#define TE_BEAMRING			5		// connect a beam ring to two entities
#define TE_BEAMSPLINE		6		
#define TE_BEAMRINGPOINT	7
#define	TE_BEAMLASER		8		// Fades according to viewpoint
#define TE_BEAMTESLA		9

enum
{
	FBEAM_STARTENTITY		= 0x00000001,
	FBEAM_ENDENTITY			= 0x00000002,
	FBEAM_FADEIN			= 0x00000004,
	FBEAM_FADEOUT			= 0x00000008,
	FBEAM_SINENOISE			= 0x00000010,
	FBEAM_SOLID				= 0x00000020,
	FBEAM_SHADEIN			= 0x00000040,
	FBEAM_SHADEOUT			= 0x00000080,
	FBEAM_ONLYNOISEONCE		= 0x00000100,		// Only calculate our noise once
	FBEAM_NOTILE			= 0x00000200,
	FBEAM_USE_HITBOXES		= 0x00000400,		// Attachment indices represent hitbox indices instead when this is set.
	FBEAM_STARTVISIBLE		= 0x00000800,		// Has this client actually seen this beam's start entity yet?
	FBEAM_ENDVISIBLE		= 0x00001000,		// Has this client actually seen this beam's end entity yet?
	FBEAM_ISACTIVE			= 0x00002000,
	FBEAM_FOREVER			= 0x00004000,
	FBEAM_HALOBEAM			= 0x00008000,		// When drawing a beam with a halo, don't ignore the segments and endwidth
	FBEAM_REVERSED			= 0x00010000,
	NUM_BEAM_FLAGS = 17	// KEEP THIS UPDATED!
};

struct BeamInfo_t
{
	int m_nType;
	C_BaseEntity *m_pStartEnt;
	int m_nStartAttachment;
	C_BaseEntity *m_pEndEnt;
	int m_nEndAttachment;
	Vector m_vecStart;
	Vector m_vecEnd;
	int m_nModelIndex;
	const char *m_pszModelName;
	int m_nHaloIndex;
	const char *m_pszHaloName;
	float m_flHaloScale;
	float m_flLife;
	float m_flWidth;
	float m_flEndWidth;
	float m_flFadeLength;
	float m_flAmplitude;
	float m_flBrightness;
	float m_flSpeed;
	int m_nStartFrame;
	float m_flFrameRate;
	float m_flRed;
	float m_flGreen;
	float m_flBlue;
	bool m_bRenderable;
	int m_nSegments;
	int m_nFlags;
	Vector m_vecCenter;
	float m_flStartRadius;
	float m_flEndRadius;

	/*BeamInfo_t()
	{
		m_nType = TE_BEAMPOINTS;
		m_nSegments = -1;
		m_pszModelName = NULL;
		m_pszHaloName = NULL;
		m_nModelIndex = -1;
		m_nHaloIndex = -1;
		m_bRenderable = true;
		m_nFlags = 0;
	}*/
};

struct Beam_t;

class C_Beam : public C_BaseEntity
{

};

class IViewRenderBeams
{
public:
	virtual void InitBeams(void) = 0;
	virtual void ShutdownBeams(void) = 0;
	virtual void ClearBeams(void) = 0;
	virtual void UpdateTempEntBeams() = 0;
	virtual void DrawBeam(C_Beam *pbeam, ITraceFilter *pEntityBeamTraceFilter = nullptr) = 0;
	virtual void DrawBeam(Beam_t *pbeam) = 0;
	virtual void KillDeadBeams(CBaseEntity *pEnt) = 0;
	virtual Beam_t *CreateBeamEnts(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t *CreateBeamEntPoint(BeamInfo_t &beamInfo) = 0;
	virtual	Beam_t *CreateBeamPoints(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t *CreateBeamRing(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t *CreateBeamRingPoint(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t *CreateBeamCirclePoints(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t *CreateBeamFollow(BeamInfo_t &beamInfo) = 0;
	virtual void FreeBeam(Beam_t *pBeam) = 0;
	virtual void UpdateBeamInfo(Beam_t *pBeam, BeamInfo_t &beamInfo) = 0;

	virtual void CreateBeamEnts(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int type = -1) = 0;

	virtual void CreateBeamEntPoint(int	nStartEntity, const Vector *pStart, int nEndEntity, const Vector *pEnd,
		int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b) = 0;

	virtual void CreateBeamPoints(Vector &start, Vector &end, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b) = 0;

	virtual void CreateBeamRing(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags = 0) = 0;

	virtual void CreateBeamRingPoint(const Vector &center, float start_radius, float end_radius, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags = 0) = 0;

	virtual void CreateBeamCirclePoints(int type, Vector &start, Vector &end,
		int modelIndex, int haloIndex, float haloScale, float life, float width,
		float m_nEndWidth, float m_nFadeLength, float amplitude, float brightness, float speed,
		int startFrame, float framerate, float r, float g, float b) = 0;

	virtual void CreateBeamFollow(int startEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float r, float g, float b,
		float brightness) = 0;
};

MAKE_INTERFACE_SIGNATURE(IViewRenderBeams, ViewRenderBeams, "client.dll", "8B 0D ? ? ? ? 56 8B 01 FF 50 18 0F B7 96 ? ? ? ? BF ? ? ? ? 66 3B D7 74 13 8B 0D ? ? ? ? 52 8B", 2, 2);