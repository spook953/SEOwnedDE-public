#pragma once

#include "../Singleton/Singleton.h"
#include <vector>

struct InterfaceInit_t
{
	void **m_pPtr = nullptr;
	const char *m_pszDLLName = {};
	const char *m_pszVersion = {};
	int m_nOffset = -1; //if not -1 we're going to sig scan (m_sVersion is the sig)
	int m_nDereferenceCount = 0;

	InterfaceInit_t(void **pPtr, const char *sDLLName, const char *sVersion, int nOffset, int nDereferenceCount);
};

#define MAKE_INTERFACE_VERSION(type, name, dll, version) namespace I { inline type *name = nullptr; } \
namespace MAKE_INTERFACE_SCOPE \
{\
	inline InterfaceInit_t name##InterfaceInit_t(reinterpret_cast<void **>(&I::name), dll, version, -1, 0); \
}

#define MAKE_INTERFACE_SIGNATURE(type, name, dll, version, offset, deref) namespace I { inline type *name = nullptr; } \
namespace MAKE_INTERFACE_SCOPE \
{\
	inline InterfaceInit_t name##InterfaceInit_t(reinterpret_cast<void **>(&I::name), dll, version, offset, deref); \
}

#define MAKE_INTERFACE_NULL(type, name) namespace I { inline type *name = nullptr; }

class CInterfaceManager
{
private:
	std::vector<InterfaceInit_t *> m_vecInterfaces = {};

public:
	void InitializeAllInterfaces();

	inline void AddInterface(InterfaceInit_t *pInterface)
	{
		m_vecInterfaces.push_back(pInterface);
	}
};

MAKE_SINGLETON_SCOPED(CInterfaceManager, InterfaceManager, U);