#include "HookManager.h"
#include "../Assert/Assert.h"

CHook::CHook(void *pInitFunc)
{
	U::HookManager->AddInit(pInitFunc);
}

void CHookManager::InitializeAllHooks()
{
	//AssertFatal(MH_Initialize() != MH_OK);

	MH_Initialize();

	for (const auto &pInit : m_vecHookInits)
		reinterpret_cast<void(__cdecl *)()>(pInit)();

	//AssertFatal(MH_EnableHook(MH_ALL_HOOKS) != MH_OK);

	MH_EnableHook(MH_ALL_HOOKS);
}