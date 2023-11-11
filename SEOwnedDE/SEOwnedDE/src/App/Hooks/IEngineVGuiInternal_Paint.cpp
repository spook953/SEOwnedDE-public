#include "../../SDK/SDK.h"

#include "../Features/Menu/Menu.h"
#include "../Features/ESP/ESP.h"
#include "../Features/Radar/Radar.h"
#include "../Features/MiscVisuals/MiscVisuals.h"
#include "../Features/SpectatorList/SpectatorList.h"
#include "../Features/SpyCamera/SpyCamera.h"
#include "../Features/SpyWarning/SpyWarning.h"
#include "../Features/TeamWellBeing/TeamWellBeing.h"
#include "../Features/seed_pred/seed_pred.hpp"

MAKE_HOOK(
	IEngineVGuiInternal_Paint, Memory::GetVFunc(I::EngineVGui, 14),
	void, __fastcall, void *ecx, void *edx, int mode)
{
	CALL_ORIGINAL(ecx, edx, mode);

	if (mode & PAINT_UIPANELS)
	{
		H::Draw->UpdateW2SMatrix();

		I::MatSystemSurface->StartDrawing();
		{
			F::ESP->Run();
			F::TeamWellBeing->Run();
			F::MiscVisuals->ShiftBar();
			F::Radar->Run();
			F::SpectatorList->Run();
			F::MiscVisuals->AimbotFOVCircle();
			F::SpyCamera->Run();
			F::SpyWarning->Run();
			seed_pred::paint();
			F::Menu->Run();
		}
		I::MatSystemSurface->FinishDrawing();
	}
} 