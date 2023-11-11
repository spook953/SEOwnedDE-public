#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CTFPlayerShared_InCond, Signatures::CTFPlayerShared_InCond.Get(),
	bool, __fastcall, void *ecx, void *edx, ETFCond cond)
{
	if (cond == TF_COND_DISGUISED || cond == TF_COND_DISGUISING)
	{
		if (CFG::Misc_Accuracy_Improvements)
		{
			if (C_TFPlayer *pOuter = *reinterpret_cast<C_TFPlayer **>(reinterpret_cast<DWORD>(ecx) + 392))
			{
				if (auto pLocal = H::Entities->GetLocal())
				{
					if (pOuter->m_iTeamNum() != pLocal->m_iTeamNum())
						return false;
				}
			}
		}
	}

	if (cond == TF_COND_ZOOMED)
	{
		auto ShouldDoOverrides = [&]() -> bool
		{
			if (CFG::Visuals_Removals_Mode == 1)
			{
				if (auto pLocal = H::Entities->GetLocal())
				{
					if (pLocal->m_iObserverMode() == OBS_MODE_IN_EYE)
						return false;
				}
			}

			return true;
		};

		if (ShouldDoOverrides())
		{
			DWORD dwRetAddr = reinterpret_cast<DWORD>(_ReturnAddress());

			if (CFG::Visuals_Remove_Scope)
			{
				if (dwRetAddr == Signatures::InCond_HudScopePaint.Get())
					return false;

				if (CFG::Visuals_Remove_Zoom && dwRetAddr == Signatures::InCond_ViewModelShouldDraw.Get())
					return false;
			}

			if (I::Input->CAM_IsThirdPerson())
			{
				if (CFG::Visuals_Remove_Zoom)
				{
					if (dwRetAddr == Signatures::InCond_PlayerShouldDraw.Get() || dwRetAddr == Signatures::InCond_WearableShouldDraw.Get())
						return false;
				}
			}
		}
	}

	return CALL_ORIGINAL(ecx, edx, cond);
}