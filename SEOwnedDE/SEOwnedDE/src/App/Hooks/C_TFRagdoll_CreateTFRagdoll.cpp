#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	C_TFRagdoll_CreateTFRagdoll, Memory::RelToAbs(Signatures::C_TFRagdoll_CreateTFRagdoll.Get()),
	void, __fastcall, void *ecx, void *edx)
{
	if (CFG::Visuals_Disable_Ragdolls)
		return;

	if (CFG::Visuals_Ragdolls_Active)
	{
		if (auto pRagdoll = reinterpret_cast<C_TFRagdoll *>(ecx))
		{
			auto SetEffect = [&](bool bBurning, bool bElectrocuted, bool bAsh, bool bGold, bool bIce, bool bDissolve)
			{
				if (bBurning)
				{
					pRagdoll->m_bBurning() = true;
					pRagdoll->m_bElectrocuted() = false;
					pRagdoll->m_bBecomeAsh() = false;
					pRagdoll->m_bGoldRagdoll() = false;
					pRagdoll->m_bIceRagdoll() = false;
				}

				if (bElectrocuted)
				{
					pRagdoll->m_bBurning() = false;
					pRagdoll->m_bElectrocuted() = true;
					pRagdoll->m_bBecomeAsh() = false;
					pRagdoll->m_bGoldRagdoll() = false;
					pRagdoll->m_bIceRagdoll() = false;
				}

				if (bAsh)
				{
					pRagdoll->m_bBurning() = false;
					pRagdoll->m_bElectrocuted() = false;
					pRagdoll->m_bBecomeAsh() = true;
					pRagdoll->m_bGoldRagdoll() = false;
					pRagdoll->m_bIceRagdoll() = false;
				}

				if (bGold)
				{
					pRagdoll->m_bBurning() = false;
					pRagdoll->m_bElectrocuted() = false;
					pRagdoll->m_bBecomeAsh() = false;
					pRagdoll->m_bGoldRagdoll() = true;
					pRagdoll->m_bIceRagdoll() = false;
				}

				if (bIce)
				{
					pRagdoll->m_bBurning() = false;
					pRagdoll->m_bElectrocuted() = false;
					pRagdoll->m_bBecomeAsh() = false;
					pRagdoll->m_bGoldRagdoll() = false;
					pRagdoll->m_bIceRagdoll() = true;
				}

				if (bDissolve)
				{
					pRagdoll->m_bBurning() = false;
					pRagdoll->m_bElectrocuted() = false;
					pRagdoll->m_bBecomeAsh() = false;
					pRagdoll->m_bGoldRagdoll() = false;
					pRagdoll->m_bIceRagdoll() = false;
					pRagdoll->m_bDissolving() = true;
				}
			};

			int nEffect = CFG::Visuals_Ragdolls_Effect;

			if (nEffect == 7)
				nEffect = SDKUtils::RandomInt(0, 6);

			switch (nEffect)
			{
				case 1: SetEffect(true, false, false, false, false, false); break;
				case 2: SetEffect(false, true, false, false, false, false); break;
				case 3: SetEffect(false, false, true, false, false, false); break;
				case 4: SetEffect(false, false, false, true, false, false); break;
				case 5: SetEffect(false, false, false, false, true, false); break;
				case 6: SetEffect(false, false, false, false, false, true); break;
				default: break;
			}

			pRagdoll->m_vecForce() *= CFG::Visuals_Ragdolls_Force_Mult;

			if (CFG::Visuals_Ragdolls_No_Death_Anim)
				pRagdoll->m_iDamageCustom() = 0;
		}
	}

	CALL_ORIGINAL(ecx, edx);
}