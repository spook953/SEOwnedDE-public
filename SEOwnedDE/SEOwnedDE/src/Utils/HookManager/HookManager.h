#pragma once

#include "MinHook/MinHook.h"
#include "../Singleton/Singleton.h"
#include <vector>

class CHook
{
private:
	void *m_pOriginal = nullptr;

public:
	CHook(void *pInitFunc);

	inline void Create(void *pSrc, void *pDst)
	{
		MH_CreateHook(pSrc, pDst, &m_pOriginal);
	}

	template <typename fn> inline fn Original()
	{
		return reinterpret_cast<fn>(m_pOriginal);
	}
};

#define MAKE_HOOK(name, address, type, callconvo, ...) namespace Hooks \
{\
	namespace name\
	{\
		void Init(); \
		inline CHook Hook(Init); \
		using fn = type(callconvo *)(__VA_ARGS__); \
		type callconvo Func(__VA_ARGS__); \
	}\
} \
void Hooks::name::Init() { Hook.Create(reinterpret_cast<void *>(address), Func); } \
type callconvo Hooks::name::Func(__VA_ARGS__)

class CHookManager
{
private:
	std::vector<void *> m_vecHookInits = {};

public:
	void InitializeAllHooks();

	inline void FreeAllHooks()
	{
		MH_Uninitialize();
	}

	inline void AddInit(void *pInitFunc)
	{
		m_vecHookInits.push_back(pInitFunc);
	}
};

MAKE_SINGLETON_SCOPED(CHookManager, HookManager, U);

#define CALL_ORIGINAL Hook.Original<fn>()