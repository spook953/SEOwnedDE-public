#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	CTFViewModel_CalcViewModelView, Signatures::CTFViewModel_CalcViewModelView.Get(),
	void, __fastcall, void *ecx, void *edx, C_BasePlayer *owner, const Vector &eyePosition, const QAngle &eyeAngles)
{
	auto pLocal = H::Entities->GetLocal();

	if (pLocal && !pLocal->deadflag() && CFG::Visuals_ViewModel_Active)
	{
		Vec3 vForward = {}, vRight = {}, vUp = {};
		Math::AngleVectors(eyeAngles, &vForward, &vRight, &vUp);

		Vec3 vEyePosition = eyePosition + (
			(vForward * CFG::Visuals_ViewModel_Offset_Forward) +
			(vRight * CFG::Visuals_ViewModel_Offset_Right) +
			(vUp * CFG::Visuals_ViewModel_Offset_Up)
		);

		CALL_ORIGINAL(ecx, edx, owner, vEyePosition, eyeAngles);
	}

	else CALL_ORIGINAL(ecx, edx, owner, eyePosition, eyeAngles);
}