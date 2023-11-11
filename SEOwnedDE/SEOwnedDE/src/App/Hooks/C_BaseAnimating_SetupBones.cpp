#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

#include "../Features/LagRecords/LagRecords.h"

//this here belongs to boss
//bless boss

MAKE_HOOK(
	C_BaseAnimating_SetupBones, Signatures::C_BaseAnimating_SetupBones.Get(),
	bool, __fastcall, void *ecx, void *edx, matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	if (CFG::Misc_SetupBones_Optimization && !F::LagRecords->IsSettingUpBones())
	{
		auto base_ent{ reinterpret_cast<C_BaseEntity *>(reinterpret_cast<uintptr_t>(ecx) - 0x4) };

		if (base_ent)
		{
			auto GetRootMoveParent = [&]()
			{
				auto pEntity{ base_ent };
				auto pParent{ base_ent->GetMoveParent() };

				auto its{ 0 };

				while (pParent)
				{
					if (its > 32) //XD
					{
						break;
					}

					its++;

					pEntity = pParent;
					pParent = pEntity->GetMoveParent();
				}

				return pEntity;
			};

			auto owner{ GetRootMoveParent() };

			auto ent{ owner ? owner : base_ent };

			if (ent->GetClassId() == ETFClassIds::CTFPlayer && ent != H::Entities->GetLocal())
			{
				if (pBoneToWorldOut)
				{
					auto bones{ ent->As<C_BaseAnimating>()->GetCachedBoneData() };

					if (bones)
					{
						std::memcpy(pBoneToWorldOut, bones->Base(), sizeof(matrix3x4_t) * std::min(nMaxBones, bones->Count()));
					}
				}

				return true;
			}
		}
	}

	return CALL_ORIGINAL(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}