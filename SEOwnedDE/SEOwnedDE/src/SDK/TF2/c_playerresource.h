#pragma once
#include "igameresources.h"
#include "const.h"
#include "c_baseentity.h"
#include "string_t.h"

#define PLAYER_UNCONNECTED_NAME	"unconnected"
#define PLAYER_ERROR_NAME "ERRORNAME"

class C_PlayerResource : public C_BaseEntity, public IGameResources
{
public:
	NETVAR(m_iPing, void *, "CPlayerResource", "m_iPing");
	NETVAR(m_iScore, void *, "CPlayerResource", "m_iScore");
	NETVAR(m_iDeaths, void *, "CPlayerResource", "m_iDeaths");
	NETVAR(m_bConnected, void *, "CPlayerResource", "m_bConnected");
	NETVAR(m_iTeam, void *, "CPlayerResource", "m_iTeam");
	NETVAR(m_bAlive, void *, "CPlayerResource", "m_bAlive");
	NETVAR(m_iHealth, void *, "CPlayerResource", "m_iHealth");
	NETVAR(m_iAccountID, void *, "CPlayerResource", "m_iAccountID");
	NETVAR(m_bValid, void *, "CPlayerResource", "m_bValid");
	NETVAR(m_iUserID, void *, "CPlayerResource", "m_iUserID");

	int GetPing(int playerindex)
	{
		static int nOffset = NetVars::GetNetVar("CPlayerResource", "m_iPing");
		return *reinterpret_cast<int *>((reinterpret_cast<DWORD>(this) + nOffset) + (playerindex * 4));
	}

//public:
//	virtual ~C_PlayerResource();
//	virtual int GetTeamScore(int index);
//	virtual const char *GetTeamName(int index);
//	virtual const Color &GetTeamColor(int index);
//	virtual bool IsConnected(int index);
//	virtual bool IsAlive(int index);
//	virtual bool IsFakePlayer(int index);
//	virtual bool IsLocalPlayer(int index);
//	virtual bool IsHLTV(int index);
//	virtual bool IsReplay(int index);
//	virtual const char *GetPlayerName(int index);
//	virtual int GetPing(int index);
//	virtual int GetPlayerScore(int index);
//	virtual int GetDeaths(int index);
//	virtual int GetTeam(int index);
//	virtual int GetFrags(int index);
//	virtual int GetHealth(int index);
//	virtual void ClientThink();
//	virtual	void OnDataChanged(DataUpdateType_t updateType);
//
//protected:
//	string_t m_szName[MAX_PLAYERS + 1];
//	int m_iPing[MAX_PLAYERS + 1];
//	int m_iScore[MAX_PLAYERS + 1];
//	int m_iDeaths[MAX_PLAYERS + 1];
//	bool m_bConnected[MAX_PLAYERS + 1];
//	int m_iTeam[MAX_PLAYERS + 1];
//	bool m_bAlive[MAX_PLAYERS + 1];
//	int m_iHealth[MAX_PLAYERS + 1];
//	Color m_Colors[MAX_TEAMS];
//	uint32	m_iAccountID[MAX_PLAYERS + 1];
//	bool m_bValid[MAX_PLAYERS + 1];
//	string_t m_szUnconnectedName;
};