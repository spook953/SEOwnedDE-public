#pragma once
#include "interface.h"
#include "basehandle.h"
#include "iclientrenderable.h"
#include "keyvalues.h"
#include "imaterialsystem.h"

typedef unsigned int HTOOLHANDLE;

enum
{
	HTOOLHANDLE_INVALID = 0
};

enum ShadowFlags_t
{
	SHADOW_FLAGS_FLASHLIGHT = (1 << 0),
	SHADOW_FLAGS_SHADOW = (1 << 1),
	SHADOW_FLAGS_LAST_FLAG = SHADOW_FLAGS_SHADOW
};

enum ClientShadowFlags_t
{
	SHADOW_FLAGS_USE_RENDER_TO_TEXTURE = (SHADOW_FLAGS_LAST_FLAG << 1),
	SHADOW_FLAGS_ANIMATING_SOURCE = (SHADOW_FLAGS_LAST_FLAG << 2),
	SHADOW_FLAGS_USE_DEPTH_TEXTURE = (SHADOW_FLAGS_LAST_FLAG << 3),
	CLIENT_SHADOW_FLAGS_LAST_FLAG = SHADOW_FLAGS_USE_DEPTH_TEXTURE
};

typedef void *EntitySearchResult;

class IClientTools : public IBaseInterface
{
public:
	virtual HTOOLHANDLE AttachToEntity(EntitySearchResult entityToAttach) = 0;
	virtual void DetachFromEntity(EntitySearchResult entityToDetach) = 0;
	virtual bool IsValidHandle(HTOOLHANDLE handle) = 0;
	virtual int GetNumRecordables() = 0;
	virtual HTOOLHANDLE GetRecordable(int index) = 0;
	virtual EntitySearchResult NextEntity(EntitySearchResult currentEnt) = 0;
	EntitySearchResult FirstEntity() { return NextEntity(NULL); }
	virtual void SetEnabled(HTOOLHANDLE handle, bool enabled) = 0;
	virtual void SetRecording(HTOOLHANDLE handle, bool recording) = 0;
	virtual bool ShouldRecord(HTOOLHANDLE handle) = 0;
	virtual HTOOLHANDLE GetToolHandleForEntityByIndex(int entindex) = 0;
	virtual int GetModelIndex(HTOOLHANDLE handle) = 0;
	virtual const char *GetModelName(HTOOLHANDLE handle) = 0;
	virtual const char *GetClassname(HTOOLHANDLE handle) = 0;
	virtual void AddClientRenderable(IClientRenderable *pRenderable, int renderGroup) = 0;
	virtual void RemoveClientRenderable(IClientRenderable *pRenderable) = 0;
	virtual void SetRenderGroup(IClientRenderable *pRenderable, int renderGroup) = 0;
	virtual void MarkClientRenderableDirty(IClientRenderable *pRenderable) = 0;
	virtual void UpdateProjectedTexture(ClientShadowHandle_t h, bool bForce) = 0;
	virtual bool DrawSprite(IClientRenderable *pRenderable, float scale, float frame, int rendermode, int renderfx, const Color_t &color, float flProxyRadius, int *pVisHandle) = 0;
	virtual EntitySearchResult	GetLocalPlayer() = 0;
	virtual bool GetLocalPlayerEyePosition(Vector &org, QAngle &ang, float &fov) = 0;
	virtual ClientShadowHandle_t CreateShadow(CBaseHandle handle, int nFlags) = 0;
	virtual void DestroyShadow(ClientShadowHandle_t h) = 0;
	virtual ClientShadowHandle_t CreateFlashlight(const FlashlightState_t &lightState) = 0;
	virtual void DestroyFlashlight(ClientShadowHandle_t h) = 0;
	virtual void UpdateFlashlightState(ClientShadowHandle_t h, const FlashlightState_t &lightState) = 0;
	virtual void AddToDirtyShadowList(ClientShadowHandle_t h, bool force = false) = 0;
	virtual void MarkRenderToTextureShadowDirty(ClientShadowHandle_t h) = 0;
	virtual void EnableRecordingMode(bool bEnable) = 0;
	virtual bool IsInRecordingMode() const = 0;
	virtual void TriggerTempEntity(KeyValues *pKeyValues) = 0;
	virtual int GetOwningWeaponEntIndex(int entindex) = 0;
	virtual int GetEntIndex(EntitySearchResult entityToAttach) = 0;
	virtual int FindGlobalFlexcontroller(char const *name) = 0;
	virtual char const *GetGlobalFlexControllerName(int idx) = 0;
	virtual EntitySearchResult	GetOwnerEntity(EntitySearchResult currentEnt) = 0;
	virtual bool IsPlayer(EntitySearchResult currentEnt) = 0;
	virtual bool IsBaseCombatCharacter(EntitySearchResult currentEnt) = 0;
	virtual bool IsNPC(EntitySearchResult currentEnt) = 0;
	virtual Vector GetAbsOrigin(HTOOLHANDLE handle) = 0;
	virtual QAngle GetAbsAngles(HTOOLHANDLE handle) = 0;
	virtual void ReloadParticleDefintions(const char *pFileName, const void *pBufData, int nLen) = 0;
	virtual void PostToolMessage(KeyValues *pKeyValues) = 0;
	virtual void EnableParticleSystems(bool bEnable) = 0;
	virtual bool IsRenderingThirdPerson() const = 0;
};

class CClientTools : public IClientTools
{
public:
	/*void OnEntityDeleted(C_BaseEntity *pEntity) {
		reinterpret_cast<void(__thiscall *)(void *, C_BaseEntity * pEntity)>(Signatures::CClientTools_OnEntityDeleted.Get())(this, pEntity);
	}

	void OnEntityCreated(C_BaseEntity *pEntity) {
		reinterpret_cast<void(__thiscall *)(void *, C_BaseEntity * pEntity)>(Signatures::CClientTools_OnEntityCreated.Get())(this, pEntity);
	}*/
};

MAKE_INTERFACE_VERSION(CClientTools, ClientTools, "client.dll", "VCLIENTTOOLS001");