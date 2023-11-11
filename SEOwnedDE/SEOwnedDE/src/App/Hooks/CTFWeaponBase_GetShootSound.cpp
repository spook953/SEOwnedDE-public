#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CTFWeaponBase_GetShootSound, Signatures::CTFWeaponBase_GetShootSound.Get(),
	const char *, __fastcall, void *ecx, void *edx, int iIndex)
{
	if (CFG::Misc_MVM_Giant_Weapon_Sounds)
	{
		if (auto pWeapon = reinterpret_cast<C_TFWeaponBase *>(ecx))
		{
			if (auto pLocal = H::Entities->GetLocal())
			{
				if (pWeapon->m_hOwner().Get() == pLocal)
				{
					int nOldTeam = pWeapon->m_iTeamNum();
					pWeapon->m_iTeamNum() = TF_TEAM_PVE_INVADERS_GIANTS;
					auto ret = CALL_ORIGINAL(ecx, edx, iIndex);
					pWeapon->m_iTeamNum() = nOldTeam;

					//credits: KGB

					static auto FireHash = HASH_CT("Weapon_FlameThrower.Fire");

					if (HASH_RT(ret) == FireHash)
						return "MVM.GiantPyro_FlameStart";

					static auto FireLoopHash = HASH_CT("Weapon_FlameThrower.FireLoop");

					if (HASH_RT(ret) == FireLoopHash)
						return "MVM.GiantPyro_FlameLoop";

					static auto GrenadeHash = HASH_CT("Weapon_GrenadeLauncher.Single");

					if (HASH_RT(ret) == GrenadeHash)
						return "MVM.GiantDemoman_Grenadeshoot";

					return ret;
				}
			}
		}
	}

	return CALL_ORIGINAL(ecx, edx, iIndex);
}