#pragma once
#include "iclientmode.h"
#include "GameEventListener.h"

class CBaseHudChat;
class CBaseHudWeaponSelection;
class CViewSetup;
class C_BaseEntity;
class C_BasePlayer;
class CBaseViewport;
class CReplayReminderPanel;

MAKE_SIGNATURE(PrintToChat, "client.dll", "55 8B EC 81 EC 00 0C 00 00 B9 ? ? ? ? 56 68 ? ? ? ? E8 ? ? ? ? 8B F0 85 F6 74 ? 68 00 08 00 00 8D 85 00 F4 FF FF 6A 00 50 E8 ? ? ? ? 8B 0D ? ? ? ? 8D 95 00 F4 FF FF 83 C4 0C 8B 01 FF 75 0C", 0);

class CClientModeShared : public IClientMode, public CGameEventListener
{
public:
	virtual ~CClientModeShared();
	virtual void Init();
	virtual void InitViewport();
	virtual void VGui_Shutdown();
	virtual void Shutdown();
	virtual void LevelInit(const char *newmap);
	virtual void LevelShutdown(void);
	virtual void Enable();
	virtual void Disable();
	virtual void Layout();
	virtual void ReloadScheme(bool flushLowLevel);
	virtual void OverrideView(CViewSetup *pSetup);
	virtual bool ShouldDrawDetailObjects();
	virtual bool ShouldDrawEntity(C_BaseEntity *pEnt);
	virtual bool ShouldDrawLocalPlayer(C_BasePlayer *pPlayer);
	virtual bool ShouldDrawViewModel();
	virtual bool ShouldDrawParticles();
	virtual bool ShouldDrawCrosshair(void);
	virtual bool ShouldBlackoutAroundHUD() override;
	virtual HeadtrackMovementMode_t ShouldOverrideHeadtrackControl() override;
	virtual void AdjustEngineViewport(int &x, int &y, int &width, int &height);
	virtual void PreRender(CViewSetup *pSetup);
	virtual void PostRender();
	virtual void PostRenderVGui();
	virtual void ProcessInput(bool bActive);
	virtual bool CreateMove(float flInputSampleTime, CUserCmd *cmd);
	virtual void Update();
	virtual int KeyInput(int down, ButtonCode_t keynum, const char *pszCurrentBinding);
	virtual int HudElementKeyInput(int down, ButtonCode_t keynum, const char *pszCurrentBinding);
	virtual void OverrideMouseInput(float *x, float *y);
	virtual void StartMessageMode(int iMessageModeType);
	virtual Panel *GetMessagePanel();
	virtual void ActivateInGameVGuiContext(Panel *pPanel);
	virtual void DeactivateInGameVGuiContext();
	virtual bool ShouldDrawFog(void);
	virtual float GetViewModelFOV(void);
	virtual Panel *GetViewport() { }
	virtual AnimationController *GetViewportAnimationController() {}
	virtual void FireGameEvent(IGameEvent *event);
	virtual bool CanRecordDemo(char *errorMsg, int length) const { return true; }
	virtual int HandleSpectatorKeyInput(int down, ButtonCode_t keynum, const char *pszCurrentBinding);
	virtual void ComputeVguiResConditions(KeyValues *pkvConditions) override;
	virtual wchar_t *GetServerName() { return nullptr; }
	virtual void SetServerName(wchar_t *name) {}
	virtual wchar_t *GetMapName() { return nullptr; }
	virtual void SetMapName(wchar_t *name) {}
	virtual bool DoPostScreenSpaceEffects(const CViewSetup *pSetup);
	virtual void DisplayReplayMessage(const char *pLocalizeName, float flDuration, bool bUrgent, const char *pSound, bool bDlg);
	virtual bool IsInfoPanelAllowed() override { return true; }
	virtual void InfoPanelDisplayed() override { }
	virtual bool IsHTMLInfoPanelAllowed() override { return true; }
	virtual void OnDemoRecordStart(char const *pDemoBaseName) override {}
	virtual void OnDemoRecordStop() override {}

	void PrintToChat(const char *pText)
	{
		reinterpret_cast<void(__thiscall *)(void *, const char *, KeyValues *)>(Signatures::PrintToChat.Get())(this, pText, nullptr);
	}
};

MAKE_INTERFACE_SIGNATURE(CClientModeShared, ClientModeShared, "client.dll", "8B 0D ? ? ? ? 8B 02 D9 05", 2, 2);