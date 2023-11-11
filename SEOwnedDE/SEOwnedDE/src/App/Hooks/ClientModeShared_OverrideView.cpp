#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

#include "../Features/AimUtils/AimUtils.h"

void DoFOV(CViewSetup *pSetup)
{
	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal)
		return;

	if (CFG::Visuals_Removals_Mode == 1 && pLocal->m_iObserverMode() == OBS_MODE_IN_EYE)
		return;

	if (!CFG::Visuals_Remove_Zoom && pLocal->IsZoomed())
		return;

	if (!pLocal->deadflag())
		pLocal->m_iFOV() = static_cast<int>(CFG::Visuals_FOV_Override);

	pSetup->fov = CFG::Visuals_FOV_Override;
}

void DoThirdperson(CViewSetup *pSetup)
{
	auto pLocal = H::Entities->GetLocal();

	if (!pLocal || pLocal->deadflag())
		return;

	if (!I::MatSystemSurface->IsCursorVisible() && !I::EngineVGui->IsGameUIVisible() && !SDKUtils::BInEndOfMatch() && !G::bStartedFakeTaunt)
	{
		if (H::Input->IsPressed(CFG::Visuals_Thirdperson_Key))
			CFG::Visuals_Thirdperson_Active = !CFG::Visuals_Thirdperson_Active;
	}

	bool bShouldDoTP = CFG::Visuals_Thirdperson_Active
		|| pLocal->InCond(TF_COND_TAUNTING)
		|| pLocal->InCond(TF_COND_HALLOWEEN_KART)
		|| pLocal->InCond(TF_COND_HALLOWEEN_THRILLER)
		|| pLocal->InCond(TF_COND_HALLOWEEN_GHOST_MODE)
		|| G::bStartedFakeTaunt;

	if (bShouldDoTP)
	{
		I::Input->CAM_ToThirdPerson();
	}

	else
	{
		I::Input->CAM_ToFirstPerson();
	}

	pLocal->ThirdPersonSwitch();

	if (bShouldDoTP)
	{
		Vec3 vForward = {}, vRight = {}, vUp = {};
		Math::AngleVectors(pSetup->angles, &vForward, &vRight, &vUp);

		Vec3 vOffset = (vForward * CFG::Visuals_Thirdperson_Offset_Forward)
			- (vRight * CFG::Visuals_Thirdperson_Offset_Right)
			- (vUp * CFG::Visuals_Thirdperson_Offset_Up);

		Vec3 vDesiredOrigin = pSetup->origin - vOffset;

		Ray_t Ray = {};
		Ray.Init(pSetup->origin, vDesiredOrigin, { -10.0f, -10.0f, -10.0f }, { 10.0f, 10.0f, 10.0f });
		CTraceFilterWorldCustom Filter = {};
		trace_t Trace = {};
		I::EngineTrace->TraceRay(Ray, MASK_SOLID, &Filter, &Trace);

		pSetup->origin -= vOffset * Trace.fraction;
	}
}

MAKE_HOOK(
	ClientModeShared_OverrideView, Memory::GetVFunc(I::ClientModeShared, 16),
	void, __fastcall, void *ecx, void *edx, CViewSetup *pSetup)
{
	if (!pSetup)
	{
		return;
	}

	//CALL_ORIGINAL(ecx, edx, pSetup);

	DoFOV(pSetup);
	DoThirdperson(pSetup);
}