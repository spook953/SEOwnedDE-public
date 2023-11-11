#include "../../SDK/SDK.h"

#include "../Features/Materials/Materials.h"
#include "../Features/Outlines/Outlines.h"
#include "../Features/WorldModulation/WorldModulation.h"
#include "../Features/Paint/Paint.h"
#include "../Features/seed_pred/seed_pred.hpp"

MAKE_HOOK(
	IBaseClientDLL_LevelShutdown, Memory::GetVFunc(I::BaseClientDLL, 7),
	void, __fastcall, void *ecx, void *edx)
{
	CALL_ORIGINAL(ecx, edx);

	H::Entities->ClearCache();
	H::Entities->ClearModelIndexes();

	F::Materials->CleanUp();
	F::Outlines->CleanUp();
	F::Paint->CleanUp();
	F::WorldModulation->LevelShutdown();

	seed_pred::reset();

	G::mapVelFixRecords.clear();

	Shifting::Reset();
}