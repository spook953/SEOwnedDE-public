#include "NetworkFix.h"

#include "../CFG.h"

MAKE_HOOK(
	CL_ReadPackets, Signatures::CL_ReadPackets.Get(),
	void, __cdecl, bool bFinalTick)
{
	if (!CFG::Misc_Ping_Reducer)
	{
		CALL_ORIGINAL(bFinalTick);

		return;
	}

	if (F::NetworkFix->ShouldReadPackets())
	{
		CALL_ORIGINAL(bFinalTick);
	}
}

void CReadPacketState::Store()
{
	m_flFrameTimeClientState = I::ClientState->m_frameTime;
	m_flFrameTime = I::GlobalVars->frametime;
	m_flCurTime = I::GlobalVars->curtime;
	m_nTickCount = I::GlobalVars->tickcount;
}

void CReadPacketState::Restore()
{
	I::ClientState->m_frameTime = m_flFrameTimeClientState;
	I::GlobalVars->frametime = m_flFrameTime;
	I::GlobalVars->curtime = m_flCurTime;
	I::GlobalVars->tickcount = m_nTickCount;
}

void CNetworkFix::FixInputDelay(bool bFinalTick)
{
	if (!I::EngineClient->IsInGame())
	{
		return;
	}

	if (auto pNetChannel = I::EngineClient->GetNetChannelInfo())
	{
		if (pNetChannel->IsLoopback())
		{
			return;
		}
	}

	CReadPacketState Backup = {};

	Backup.Store();

	Hooks::CL_ReadPackets::Hook.Original<Hooks::CL_ReadPackets::fn>()(bFinalTick);

	m_State.Store();

	Backup.Restore();
}

bool CNetworkFix::ShouldReadPackets()
{
	if (!I::EngineClient->IsInGame())
		return true;

	if (auto pNetChannel = I::EngineClient->GetNetChannelInfo())
	{
		if (pNetChannel->IsLoopback())
			return true;
	}

	m_State.Restore();

	return false;
}