#include "MiscVisuals.h"

#include "../CFG.h"
#include "../AimUtils/AimUtils.h"
#include "../RapidFire/RapidFire.h"

void CMiscVisuals::AimbotFOVCircle()
{
	if (I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	if (!CFG::Visuals_Aimbot_FOV_Circle
		|| I::EngineVGui->IsGameUIVisible()
		|| !G::CHudTFCrosshair_ShouldDraw_Result
		|| I::Input->CAM_IsThirdPerson())
		return;

	if (auto pLocal = H::Entities->GetLocal())
	{
		if (auto flAimFOV = G::flAimbotFOV) {
			float flRadius = tanf(DEG2RAD(flAimFOV) / 2.0f) / tanf(DEG2RAD(static_cast<float>(pLocal->m_iFOV())) / 2.0f) * H::Draw->GetScreenW();
			H::Draw->OutlinedCircle(H::Draw->GetScreenW() / 2, H::Draw->GetScreenH() / 2, static_cast<int>(flRadius), 70, { 255, 255, 255, static_cast<byte>(255.0f * CFG::Visuals_Aimbot_FOV_Circle_Alpha)});
		}
	}
}

void CMiscVisuals::ViewModelSway()
{
	static ConVar *cl_wpn_sway_interp = I::CVar->FindVar("cl_wpn_sway_interp");

	if (!cl_wpn_sway_interp)
		return;

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal)
		return;

	if (CFG::Visuals_ViewModel_Active && CFG::Visuals_ViewModel_Sway && !pLocal->deadflag())
	{
		if (auto pWeapon = H::Entities->GetWeapon())
		{
			float flBaseValue = pWeapon->GetWeaponID() == TF_WEAPON_COMPOUND_BOW ? 0.02f : 0.05f;

			cl_wpn_sway_interp->SetValue(flBaseValue * CFG::Visuals_ViewModel_Sway_Scale);
		}
	}

	else
	{
		if (cl_wpn_sway_interp->GetFloat())
			cl_wpn_sway_interp->SetValue(0.0f);
	}
}

void CMiscVisuals::DetailProps()
{
	if (!CFG::Visuals_Disable_Detail_Props)
		return;

	static ConVar *r_drawdetailprops = I::CVar->FindVar("r_drawdetailprops");

	if (r_drawdetailprops && r_drawdetailprops->GetInt())
		r_drawdetailprops->SetValue(0);
}

void CMiscVisuals::ShiftBar()
{
	if (!CFG::Exploits_Shifting_Draw_Indicator)
		return;

	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	if (I::EngineVGui->IsGameUIVisible() || !G::CHudTFCrosshair_ShouldDraw_Result || SDKUtils::BInEndOfMatch())
		return;

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal || pLocal->deadflag())
		return;

	auto pWeapon = H::Entities->GetWeapon();

	if (!pWeapon)
		return;

	static int nBarW = 80;
	static int nBarH = 4;

	int nBarX = (H::Draw->GetScreenW() / 2) - (nBarW / 2);
	int nBarY = (H::Draw->GetScreenH() / 2) + 100;
	int circle_x{ H::Draw->GetScreenW() / 2 };

	if (CFG::Exploits_Shifting_Indicator_Style == 0)
	{
		H::Draw->Rect(nBarX - 1, nBarY - 1, nBarW + 2, nBarH + 2, CFG::Menu_Background);

		if (Shifting::nAvailableTicks > 0)
		{
			Color_t Color = CFG::Menu_Accent_Secondary;
			Color_t ColorDim = { Color.r, Color.g, Color.b, 25 };

			int nFillWidth = static_cast<int>(Math::RemapValClamped(
				static_cast<float>(Shifting::nAvailableTicks),
				0.0f, static_cast<float>(MAX_COMMANDS),
				0.0f, static_cast<float>(nBarW)
			));

			H::Draw->GradientRect(nBarX, nBarY, nFillWidth, nBarH, ColorDim, Color, false);
			H::Draw->OutlinedRect(nBarX, nBarY, nFillWidth, nBarH, Color);
		}
	}

	if (CFG::Exploits_Shifting_Indicator_Style == 1)
	{
		float end{ Math::RemapValClamped(static_cast<float>(Shifting::nAvailableTicks), 0.0f, MAX_COMMANDS, -90.0f, 359.0f) };

		H::Draw->Arc(circle_x, nBarY, 21, 6.0f, -90.0f, 359.0f, CFG::Menu_Background);
		H::Draw->Arc(circle_x, nBarY, 20, 4.0f, -90.0f, end, CFG::Menu_Accent_Secondary);
	}

	if (G::nTicksSinceCanFire < 30 && F::RapidFire->IsWeaponSupported(pWeapon))
	{
		if (CFG::Exploits_Shifting_Indicator_Style == 0)
		{
			H::Draw->Rect(nBarX - 1, (nBarY + nBarH + 4) - 1, nBarW + 2, nBarH + 2, CFG::Menu_Background);

			if (G::nTicksSinceCanFire > 0)
			{
				Color_t Color = { 241, 196, 15, 255 };
				Color_t ColorDim = { Color.r, Color.g, Color.b, 25 };

				int nFillWidth = static_cast<int>(Math::RemapValClamped(
					static_cast<float>(G::nTicksSinceCanFire),
					0.0f, 24.0f,
					0.0f, static_cast<float>(nBarW)
				));

				H::Draw->GradientRect(nBarX, nBarY + nBarH + 4, nFillWidth, nBarH, ColorDim, Color, false);
				H::Draw->OutlinedRect(nBarX, nBarY + nBarH + 4, nFillWidth, nBarH, Color);
			}

		}

		if (CFG::Exploits_Shifting_Indicator_Style == 1)
		{
			float end{ Math::RemapValClamped(static_cast<float>(G::nTicksSinceCanFire), 0.0f, 24.0f, -90.0f, 359.0f) };

			H::Draw->Arc(circle_x, nBarY, 24, 2.0f, -90.0f, 359.0f, CFG::Menu_Background);
			H::Draw->Arc(circle_x, nBarY, 24, 2.0f, -90.0f, end, { 241, 196, 15, 255 });
		}
	}
}