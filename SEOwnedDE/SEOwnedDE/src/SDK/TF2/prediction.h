#pragma once
#include "iprediction.h"
#include "c_baseplayer.h"
#include "imovehelper.h"

class CMoveData;
class CUserCmd;
class C_BaseEntity;

class CPrediction : public IPrediction
{
public:
	virtual ~CPrediction();
	virtual void Init();
	virtual void Shutdown();
	virtual void Update(int startframe, bool validframe, int incoming_acknowledged, int outgoing_command);
	virtual void OnReceivedUncompressedPacket();
	virtual void PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet);
	virtual void PostEntityPacketReceived();
	virtual void PostNetworkDataReceived(int commands_acknowledged);
	virtual bool InPrediction();
	virtual bool IsFirstTimePredicted();
	virtual int	GetIncomingPacketNumber();
	virtual void GetViewOrigin(Vec3 &org);
	virtual void SetViewOrigin(Vec3 &org);
	virtual void GetViewAngles(QAngle &ang);
	virtual void SetViewAngles(QAngle &ang);
	virtual void GetLocalViewAngles(QAngle &ang);
	virtual void SetLocalViewAngles(QAngle &ang);
	virtual void RunCommand(C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper);
	virtual void SetupMove(C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move);
	virtual void FinishMove(C_BasePlayer *player, CUserCmd *ucmd, CMoveData *move);
	virtual void SetIdealPitch(C_BasePlayer *player, const Vec3 &origin, const Vec3 &angles, const Vec3 &viewheight);
	virtual void _Update(bool received_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command);

	CHandle<C_BaseEntity> m_hLastGround;
	bool m_bInPrediction;
	bool m_bFirstTimePredicted;
	bool m_bOldCLPredictValue;
	bool m_bEnginePaused;
	int m_nPreviousStartFrame;
	int m_nCommandsPredicted;
	int m_nServerCommandsAcknowledged;
	int m_bPreviousAckHadErrors;
	int m_nIncomingPacketNumber;
	float m_flIdealPitch;
};


MAKE_INTERFACE_VERSION(CPrediction, Prediction, "client.dll", "VClientPrediction001");