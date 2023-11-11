#pragma once
#include "Signatures.h"
#include "../../Utils/Utils.h"

class IBaseInterface
{
public:
	virtual	~IBaseInterface() {}
};

typedef void *(*CreateInterfaceFn)(const char *pName, int *pReturnCode);
typedef void *(*InstantiateInterfaceFn)();

class InterfaceReg
{
public:
	InstantiateInterfaceFn	m_CreateFn = {};
	const char *m_pName = {};
	InterfaceReg *m_pNext = {};
	static InterfaceReg *s_pInterfaceRegs;
};

class CSysModule;

enum
{
	IFACE_OK = 0,
	IFACE_FAILED
};

enum Sys_Flags
{
	SYS_NOFLAGS = 0x00,
	SYS_NOLOAD = 0x01
};

class CDllDemandLoader
{
private:
	char const *m_pchModuleName = {};
	CSysModule *m_hModule = {};
	bool m_bLoadAttempted = {};
};

namespace SDKUtils
{
	inline float AttribHookValue(float value, const char *name, void *econent) {
		return reinterpret_cast<float(__cdecl *)(float, const char *, void *, void *, bool)>(Signatures::AttribHookValue.Get())(value, name, econent, nullptr, true);
	}
}