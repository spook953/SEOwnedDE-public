#pragma once

#include "inetchannel.h"

class CClockDriftMgr
{
public:
	enum { NUM_CLOCKDRIFT_SAMPLES = 16 };

	float m_ClockOffsets[NUM_CLOCKDRIFT_SAMPLES]{};
	int m_iCurClockOffset{};
	int m_nServerTick{};
	int	m_nClientTick{};
};

class CClientState
{
public:
	byte pad0[0x10]{};
	INetChannel *m_NetChannel{};
	byte pad1[0x140]{};
	CClockDriftMgr m_ClockDriftMgr{};
	int m_nDeltaTick{};
	byte pad2[0x110]{};
	int m_nMaxClients{};
	byte pad3[0x4868]{};
	float m_frameTime{};
	int lastoutgoingcommand{};
	int chokedcommands{};
	int last_command_ack{};

public:
	void SendStringCmd(const char *command)
	{
		reinterpret_cast<void(__thiscall *)(void *, const char *)>(Signatures::CBaseClientState_SendStringCmd.Get())(this, command);
	}
};

MAKE_INTERFACE_SIGNATURE(CClientState, ClientState, "engine.dll", "68 ? ? ? ? E8 ? ? ? ? 83 C4 08 5F 5E 5B 5D C3", 1, 1);