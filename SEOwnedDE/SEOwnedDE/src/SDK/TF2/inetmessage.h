#pragma once
#include "bitbuf.h"

class INetMessage
{
public:
	virtual	~INetMessage() {};
	virtual void SetNetChannel(void *netchan) = 0;
	virtual void SetReliable(bool state) = 0;
	virtual bool Process(void) = 0;
	virtual	bool ReadFromBuffer(bf_read &buffer) = 0;
	virtual	bool WriteToBuffer(bf_write &buffer) = 0;
	virtual bool IsReliable(void) const = 0;
	virtual int GetType(void) const = 0;
	virtual int GetGroup(void) const = 0;
	virtual const char *GetName(void) const = 0;
	virtual void *GetNetChannel(void) const = 0;
	virtual const char *ToString(void) const = 0;
};