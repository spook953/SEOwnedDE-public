#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	R_ComputeLightingOrigin, Signatures::R_ComputeLightingOrigin.Get(),
	void, __cdecl, IClientRenderable *pRenderable, studiohdr_t *pStudioHdr, const matrix3x4_t &matrix, Vector &center)
{
	if (CFG::Misc_ComputeLightingOrigin_Fix)
	{
		if (!I::EngineClient->IsInGame() || !I::EngineClient->IsConnected() || !H::Entities->GetLocal())
		{
			CALL_ORIGINAL(pRenderable, pStudioHdr, matrix, center);

			return;
		}

		if (auto pEntity = static_cast<C_BaseEntity *>(pRenderable))
		{
			if (auto pOwner = pEntity->m_hOwnerEntity().Get())
			{
				if (pOwner->GetClassId() == ETFClassIds::CTFPlayer)
				{
					center = pOwner->As<C_TFPlayer>()->GetRenderCenter();

					return;
				}
			}

			else
			{
				if (pEntity->GetClassId() == ETFClassIds::CTFPlayer)
				{
					center = pEntity->As<C_TFPlayer>()->GetRenderCenter();

					return;
				}
			}
		}
	}

	CALL_ORIGINAL(pRenderable, pStudioHdr, matrix, center);
}