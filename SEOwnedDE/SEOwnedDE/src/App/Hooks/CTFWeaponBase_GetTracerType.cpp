#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CTFWeaponBase_GetTracerType, Signatures::CTFWeaponBase_GetTracerType.Get(),
	const char *, __fastcall, void *ecx, void *edx)
{
	if (auto nType = CFG::Visuals_Tracer_Type)
	{
		if (auto pLocal = H::Entities->GetLocal())
		{
			if (auto pWeapon = reinterpret_cast<C_TFWeaponBase *>(ecx))
			{
				if (pWeapon->m_hOwner().Get() == pLocal)
				{
					auto GetTracerName = [&](int nIndex)
					{
						switch (nIndex)
						{
							case 1: return pLocal->m_iTeamNum() == 2 ? "bullet_tracer_raygun_red" : "bullet_tracer_raygun_blue";
							case 2: return "dxhr_sniper_rail";
							case 3: return pLocal->m_iTeamNum() == 2 ? "dxhr_sniper_rail_red" : "dxhr_sniper_rail_blue";
							case 4: return pLocal->m_iTeamNum() == 2 ? "bullet_bignasty_tracer01_red" : "bullet_bignasty_tracer01_blue";
							case 5: return pLocal->m_iTeamNum() == 2 ? "dxhr_lightningball_hit_zap_red" : "dxhr_lightningball_hit_zap_blue";
							case 6: return "merasmus_zap";
							default: return CALL_ORIGINAL(ecx, edx);
						}
					};

					bool bIsTF2DevsHorribleCode = pWeapon->m_iItemDefinitionIndex() == Sniper_m_TheMachina
						|| pWeapon->m_iItemDefinitionIndex() == Sniper_m_ShootingStar;

					return GetTracerName((nType > 6) ? SDKUtils::RandomInt(bIsTF2DevsHorribleCode ? 1 : 0, (nType == 8) ? 5 : 6) : nType);
				}
			}
		}
	}

	return CALL_ORIGINAL(ecx, edx);
}