#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CBaseEntity_BaseInterpolatePart1, Memory::RelToAbs(Signatures::CBaseEntity_BaseInterpolatePart1.Get()),
	int, __fastcall, void *ecx, void *edx, float &currentTime, Vector &oldOrigin, QAngle &oldAngles, Vector &oldVel, int &bNoMoreChanges)
{
	auto ShouldDisableInterp = [&]()
	{
		if (!CFG::Misc_Accuracy_Improvements)
		{
			auto pLocal = H::Entities->GetLocal();

			if (!pLocal)
				return false;

			auto pEntity = reinterpret_cast<C_BaseEntity *>(ecx);

			if (!pEntity)
				return false;

			if (pEntity == pLocal)
				return Shifting::bRecharging;

			return false;
		}

		auto pLocal = H::Entities->GetLocal();

		if (!pLocal)
			return false;

		auto pEntity = reinterpret_cast<C_BaseEntity *>(ecx);

		if (!pEntity)
			return false;
		
		if (pEntity == pLocal)
			return Shifting::bRecharging;

		if (pEntity->GetClassId() == ETFClassIds::CTFPlayer)
			return pEntity != pLocal;

		if (pEntity->GetClassId() == ETFClassIds::CBaseDoor)
			return true;

		return false;
	};

	if (ShouldDisableInterp())
	{
		bNoMoreChanges = 1;

		return 0;
	}

	return CALL_ORIGINAL(ecx, edx, currentTime, oldOrigin, oldAngles, oldVel, bNoMoreChanges);
}