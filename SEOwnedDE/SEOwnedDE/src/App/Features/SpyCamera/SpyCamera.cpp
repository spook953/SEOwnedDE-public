#include "SpyCamera.h"

#include "../CFG.h"
#include "../Menu/Menu.h"
#include "../AimUtils/AimUtils.h"

void CSpyCamera::Drag()
{
	int nMouseX = H::Input->GetMouseX();
	int nMouseY = H::Input->GetMouseY();

	bool bHovered = false;
	static bool bDragging = false;

	if (!bDragging && F::Menu->IsMenuWindowHovered())
		return;

	static int nDeltaX = 0;
	static int nDeltaY = 0;

	int nCamX = CFG::Visuals_SpyCamera_Pos_X;
	int nCamY = CFG::Visuals_SpyCamera_Pos_Y;
	int nCamW = CFG::Visuals_SpyCamera_Pos_W;

	bHovered = nMouseX > nCamX && nMouseX < nCamX + nCamW && nMouseY > nCamY && nMouseY < nCamY + CFG::Menu_Drag_Bar_Height;

	if (bHovered && H::Input->IsPressed(VK_LBUTTON)) {
		nDeltaX = nMouseX - nCamX;
		nDeltaY = nMouseY - nCamY;
		bDragging = true;
	}

	if (!H::Input->IsPressed(VK_LBUTTON) && !H::Input->IsHeld(VK_LBUTTON))
		bDragging = false;

	if (bDragging) {
		CFG::Visuals_SpyCamera_Pos_X = nMouseX - nDeltaX;
		CFG::Visuals_SpyCamera_Pos_Y = nMouseY - nDeltaY;
	}
}

void CSpyCamera::Run()
{
	if (!CFG::Visuals_SpyCamera_Active)
		return;

	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	if (!F::Menu->IsOpen() && (I::EngineVGui->IsGameUIVisible() || SDKUtils::BInEndOfMatch()))
		return;

	if (F::Menu->IsOpen())
		Drag();

	auto BackgroundColor = []() -> Color_t {
		Color_t Out = CFG::Menu_Background;
		Out.a = static_cast<byte>(CFG::Visuals_SpyCamera_Background_Alpha * 255.0f);
		return Out;
	}();

	H::Draw->Rect(
		CFG::Visuals_SpyCamera_Pos_X,
		CFG::Visuals_SpyCamera_Pos_Y,
		CFG::Visuals_SpyCamera_Pos_W,
		CFG::Menu_Drag_Bar_Height,
		BackgroundColor
	);

	H::Draw->OutlinedRect(
		CFG::Visuals_SpyCamera_Pos_X,
		CFG::Visuals_SpyCamera_Pos_Y,
		CFG::Visuals_SpyCamera_Pos_W,
		CFG::Menu_Drag_Bar_Height,
		CFG::Menu_Accent_Secondary
	);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		CFG::Visuals_SpyCamera_Pos_X + (CFG::Visuals_SpyCamera_Pos_W / 2),
		CFG::Visuals_SpyCamera_Pos_Y + (CFG::Menu_Drag_Bar_Height / 2),
		CFG::Menu_Text,
		POS_CENTERXY,
		"Spy Camera"
	);

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal || pLocal->deadflag() || !I::ViewRender)
		return;

	std::vector<C_TFPlayer *> vecSpies = {};

	for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ENEMIES))
	{
		if (!pEntity)
			continue;

		auto pPlayer = pEntity->As<C_TFPlayer>();

		if (!pPlayer || pPlayer->deadflag() || pPlayer->m_iClass() != TF_CLASS_SPY || pPlayer->InCond(TF_COND_STEALTHED))
			continue;

		if (pPlayer->GetCenter().DistTo(pLocal->GetCenter()) > 400.0f)
			continue;

		Vec3 vEngineAngles = I::EngineClient->GetViewAngles();

		if (Math::CalcFov({ 0.0f, vEngineAngles.y, 0.0f }, Math::CalcAngle(pLocal->GetShootPos(), pPlayer->GetCenter())) < 80.0f)
			continue;

		if (!F::AimUtils->TraceEntityAutoDet(pPlayer, pLocal->GetShootPos(), pPlayer->GetShootPos()))
			continue;

		vecSpies.push_back(pPlayer);
	}

	if (vecSpies.empty())
		return;

	if (auto pSpy = *std::min_element(vecSpies.begin(), vecSpies.end(), [&](C_TFPlayer *a, C_TFPlayer *b) {
		return a->GetCenter().DistTo(pLocal->GetCenter()) < b->GetCenter().DistTo(pLocal->GetCenter()); }))
	{
		auto setup = G::View;

		setup.x = CFG::Visuals_SpyCamera_Pos_X + 1;
		setup.y = CFG::Visuals_SpyCamera_Pos_Y + CFG::Menu_Drag_Bar_Height;
		setup.width = CFG::Visuals_SpyCamera_Pos_W - 2;
		setup.height = CFG::Visuals_SpyCamera_Pos_H;

		Vec3 vSpyPos = pSpy->GetAbsOrigin() + Vec3(0.0f, 0.0f, pSpy->m_vecMaxs().z);

		Vec3 vAngles = Math::CalcAngle(vSpyPos, pLocal->GetCenter());
		Vec3 vForward = {};
		Math::AngleVectors(vAngles, &vForward);

		trace_t Trace = {};
		CTraceFilterWorldCustom Filter = {};
		F::AimUtils->Trace(vSpyPos, vSpyPos - (vForward * 80.0f), MASK_SOLID, &Filter, &Trace);

		setup.origin = vSpyPos - ((vForward * 80.0f) * Trace.fraction);
		setup.angles = vAngles;
		setup.fov = CFG::Visuals_SpyCamera_FOV;
		setup.m_flAspectRatio = static_cast<float>(setup.width) / static_cast<float>(setup.height);

		if (!I::Input->CAM_IsThirdPerson())
		{
			I::Input->CAM_ToThirdPerson();
		
			pLocal->UpdateVisibility();
			pLocal->CreateShadow();

			auto pAttachment = pLocal->FirstMoveChild();

			for (int n = 0; n < 32; n++)
			{
				if (!pAttachment)
					break;

				pAttachment->UpdateVisibility();
				pAttachment->CreateShadow();

				pAttachment = pAttachment->NextMovePeer();
			}
		}

		G::bRenderingSpyCamera = true;
		I::ViewRender->RenderView(setup, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_STENCIL, RENDERVIEW_UNSPECIFIED);
		G::bRenderingSpyCamera = false;

		H::Draw->OutlinedRect(
			CFG::Visuals_SpyCamera_Pos_X,
			CFG::Visuals_SpyCamera_Pos_Y,
			CFG::Visuals_SpyCamera_Pos_W,
			CFG::Visuals_SpyCamera_Pos_H + CFG::Menu_Drag_Bar_Height,
			CFG::Menu_Accent_Secondary
		);
	}
}