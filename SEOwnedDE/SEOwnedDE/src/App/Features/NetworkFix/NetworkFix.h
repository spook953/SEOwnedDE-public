#pragma once

#include "../../../SDK/SDK.h"

class CReadPacketState
{
private:
    float m_flFrameTimeClientState = 0.0f;
    float m_flFrameTime = 0.0f;
    float m_flCurTime = 0.0f;
    int m_nTickCount = 0;

public:
    void Store();
    void Restore();
};

class CNetworkFix
{
private:
    CReadPacketState m_State = {};

public:
    void FixInputDelay(bool bFinalTick);
    bool ShouldReadPackets();
};

MAKE_SINGLETON_SCOPED(CNetworkFix, NetworkFix, F);