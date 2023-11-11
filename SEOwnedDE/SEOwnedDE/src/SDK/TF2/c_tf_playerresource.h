#pragma once
#include "c_playerresource.h"
#include "interface.h"

enum MM_PlayerConnectionState_t
{
	MM_DISCONNECTED = 0,
	MM_CONNECTED,
	MM_CONNECTING,
	MM_LOADING,
	MM_WAITING_FOR_PLAYER
};

enum ETFStreak
{
	kTFStreak_Kills = 0,
	kTFStreak_KillsAll = 1,
	kTFStreak_Ducks = 2,
	kTFStreak_Duck_levelup = 3,
	kTFStreak_COUNT = 4
};

class C_TF_PlayerResource : public C_PlayerResource
{
public:
	NETVAR(m_iTotalScore, void *, "CTFPlayerResource", "m_iTotalScore");
	NETVAR(m_iMaxHealth, void *, "CTFPlayerResource", "m_iMaxHealth");
	NETVAR(m_iMaxBuffedHealth, void *, "CTFPlayerResource", "m_iMaxBuffedHealth");
	NETVAR(m_iPlayerClass, void *, "CTFPlayerResource", "m_iPlayerClass");
	NETVAR(m_bArenaSpectator, void *, "CTFPlayerResource", "m_bArenaSpectator");
	NETVAR(m_iActiveDominations, void *, "CTFPlayerResource", "m_iActiveDominations");
	NETVAR(m_flNextRespawnTime, void *, "CTFPlayerResource", "m_flNextRespawnTime");
	NETVAR(m_iChargeLevel, void *, "CTFPlayerResource", "m_iChargeLevel");
	NETVAR(m_iDamage, void *, "CTFPlayerResource", "m_iDamage");
	NETVAR(m_iDamageAssist, void *, "CTFPlayerResource", "m_iDamageAssist");
	NETVAR(m_iDamageBoss, void *, "CTFPlayerResource", "m_iDamageBoss");
	NETVAR(m_iHealing, void *, "CTFPlayerResource", "m_iHealing");
	NETVAR(m_iHealingAssist, void *, "CTFPlayerResource", "m_iHealingAssist");
	NETVAR(m_iDamageBlocked, void *, "CTFPlayerResource", "m_iDamageBlocked");
	NETVAR(m_iCurrencyCollected, void *, "CTFPlayerResource", "m_iCurrencyCollected");
	NETVAR(m_iBonusPoints, void *, "CTFPlayerResource", "m_iBonusPoints");
	NETVAR(m_iPlayerLevel, void *, "CTFPlayerResource", "m_iPlayerLevel");
	NETVAR(m_iStreaks, void *, "CTFPlayerResource", "m_iStreaks");
	NETVAR(m_iUpgradeRefundCredits, void *, "CTFPlayerResource", "m_iUpgradeRefundCredits");
	NETVAR(m_iBuybackCredits, void *, "CTFPlayerResource", "m_iBuybackCredits");
	NETVAR(m_iPartyLeaderRedTeamIndex, int, "CTFPlayerResource", "m_iPartyLeaderRedTeamIndex");
	NETVAR(m_iPartyLeaderBlueTeamIndex, int, "CTFPlayerResource", "m_iPartyLeaderBlueTeamIndex");
	NETVAR(m_iEventTeamStatus, int, "CTFPlayerResource", "m_iEventTeamStatus");
	NETVAR(m_iPlayerClassWhenKilled, void *, "CTFPlayerResource", "m_iPlayerClassWhenKilled");
	NETVAR(m_iConnectionState, void *, "CTFPlayerResource", "m_iConnectionState");
	NETVAR(m_flConnectTime, void *, "CTFPlayerResource", "m_flConnectTime");

//public:
//	virtual ~C_TF_PlayerResource();
//	virtual int	GetTeam(int index) override;
//
//private:
//	int m_iTotalScore[MAX_PLAYERS + 1];
//	int m_iMaxHealth[MAX_PLAYERS + 1];
//	int m_iMaxBuffedHealth[MAX_PLAYERS + 1];
//	int m_iPlayerClass[MAX_PLAYERS + 1];
//	bool m_bArenaSpectator[MAX_PLAYERS + 1];
//	int m_iActiveDominations[MAX_PLAYERS + 1];
//	float m_flNextRespawnTime[MAX_PLAYERS + 1];
//	int m_iChargeLevel[MAX_PLAYERS + 1];
//	int m_iDamage[MAX_PLAYERS + 1];
//	int m_iDamageAssist[MAX_PLAYERS + 1];
//	int m_iDamageBoss[MAX_PLAYERS + 1];
//	int m_iHealing[MAX_PLAYERS + 1];
//	int m_iHealingAssist[MAX_PLAYERS + 1];
//	int m_iDamageBlocked[MAX_PLAYERS + 1];
//	int m_iCurrencyCollected[MAX_PLAYERS + 1];
//	int m_iBonusPoints[MAX_PLAYERS + 1];
//	int m_iPlayerLevel[MAX_PLAYERS + 1];
//	int m_iStreaks[(MAX_PLAYERS + 1) * kTFStreak_COUNT];
//	int m_iUpgradeRefundCredits[MAX_PLAYERS + 1];
//	int m_iBuybackCredits[MAX_PLAYERS + 1];
//	int m_iPartyLeaderBlueTeamIndex;
//	int m_iPartyLeaderRedTeamIndex;
//	int m_iEventTeamStatus;
//	int m_iPlayerClassWhenKilled[MAX_PLAYERS + 1];
//
//	MM_PlayerConnectionState_t	m_iConnectionState[MAX_PLAYERS + 1];
//
//	struct PlayerScoreboardStats_t
//	{
//		int	m_iPrevDamage;
//		int	m_iPrevDamageAssist;
//		int	m_iPrevDamageBoss;
//		int	m_iPrevHealing;
//		int	m_iPrevHealingAssist;
//		int m_iPrevDamageBlocked;
//		int	m_iPrevCurrencyCollected;
//		int	m_iPrevBonusPoints;
//	};
//
//	PlayerScoreboardStats_t m_aPlayerScoreStats[MAX_PLAYERS + 1];
};

//ADD_INTERFACE_SIGNATURE(C_TF_PlayerResource, TF_PR, "client.dll", "8B 0D ? ? ? ? 6A 01 FF B6 ? ? ? ? E8 ? ? ? ? 8B 17 8B CF 8B F0 FF 92 ? ? ? ? 33 C9", 2, 2)

static C_TF_PlayerResource *GetTFPlayerResource()
{
	static auto dwPlayerResource = Memory::FindSignature("client.dll", "8B 0D ? ? ? ? 6A 01 FF B6 ? ? ? ? E8 ? ? ? ? 8B 17 8B CF 8B F0 FF 92 ? ? ? ? 33 C9");
	//Assert(dwPlayerResource == 0x0);
	return **reinterpret_cast<C_TF_PlayerResource ***>(dwPlayerResource + 2);
}