#pragma once
#include "interface.h"

enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,
	INIT_LAST_VAL
};

class IAppSystem
{
public:
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;
	virtual void *QueryInterface(const char *pInterfaceName) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
};

template< class IInterface >
class CBaseAppSystem : public IInterface
{
public:
	virtual bool Connect(CreateInterfaceFn factory) { return true; }
	virtual void Disconnect() {}
	virtual void *QueryInterface(const char *pInterfaceName) { return nullptr; }
	virtual InitReturnVal_t Init() { return INIT_OK; }
	virtual void Shutdown() {}
};

template< class IInterface >
class CTier0AppSystem : public CBaseAppSystem< IInterface >
{
public:
	CTier0AppSystem(bool bIsPrimaryAppSystem = true)
	{
		m_bIsPrimaryAppSystem = bIsPrimaryAppSystem;
	}

protected:
	bool IsPrimaryAppSystem() { return m_bIsPrimaryAppSystem; }

private:
	bool m_bIsPrimaryAppSystem = false;
};

class IAppSystemV0
{
public:
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;
	virtual void *QueryInterface(const char *pInterfaceName) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
};