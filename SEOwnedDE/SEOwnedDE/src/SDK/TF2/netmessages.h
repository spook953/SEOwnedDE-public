#pragma once
#include "inetmessage.h"
#include "inetchannel.h"

class SendTable;
class KeyValue;
class KeyValues;
class INetMessageHandler;
class IServerMessageHandler;
class IClientMessageHandler;

#define clc_ClientInfo 8
#define	clc_Move 9
#define clc_VoiceData 10 
#define clc_BaselineAck 11
#define clc_ListenEvents 12
#define clc_RespondCvarValue 13
#define clc_FileCRCCheck 14
#define clc_SaveReplay 15
#define clc_CmdKeyValues 16
#define clc_FileMD5Check 17

class CNetMessage : public INetMessage
{
public:
	virtual ~CNetMessage() {};
	virtual int GetGroup() const { return GENERIC; }
	INetChannel *GetNetChannel() { return m_NetChannel; }
	virtual void SetReliable(bool state) { m_bReliable = state; };
	virtual bool IsReliable() const { return m_bReliable; };
	virtual void SetNetChannel(INetChannel *netchan) { m_NetChannel = netchan; }
	virtual bool Process() { return false; };

protected:
	bool m_bReliable;
	INetChannel *m_NetChannel;
};

class CLC_Move
{
public:
	void *vtable;
	bool m_bReliable;
	bool m_bOwnData;
	char pad_0006[14];
	int m_nBackupCommands;
	int m_nNewCommands;
	int m_nLength;
	bf_read m_DataIn;
	bf_write m_DataOut;
};