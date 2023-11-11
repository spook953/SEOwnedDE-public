#pragma once
#include "../cdll_int.h"

namespace NetVars
{
	int GetOffset(RecvTable *pTable, const char *szNetVar);
	int GetNetVar(const char *szClass, const char *szNetVar);
}

#define NETVAR(_name, type, table, name) type &_name() \
{ \
	static int nOffset = NetVars::GetNetVar(table, name); \
	return *reinterpret_cast<type *>(reinterpret_cast<DWORD>(this) + nOffset); \
}