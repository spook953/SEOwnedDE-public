#include "SpectatorList.h"

#include "../CFG.h"
#include "../Menu/Menu.h"

#define LIST_WIDTH CFG::Visuals_SpectatorList_Width

bool CSpectatorList::GetSpectators()
{
	m_vecSpectators.clear();

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal || pLocal->deadflag())
		return false;

	for (int n = 1; n < I::EngineClient->GetMaxClients() + 1; n++)
	{
		auto pEntity = I::ClientEntityList->GetClientEntity(n);

		if (!pEntity || pEntity->IsDormant() || pEntity->GetClassId() != ETFClassIds::CTFPlayer)
			continue;

		auto pPlayer = pEntity->As<C_TFPlayer>();

		if (!pPlayer->deadflag() || pPlayer->m_hObserverTarget().Get() != pLocal)
			continue;

		int nMode = pPlayer->m_iObserverMode();

		if (nMode != OBS_MODE_IN_EYE && nMode != OBS_MODE_CHASE)
			continue;

		player_info_t PlayerInfo = {};

		if (!I::EngineClient->GetPlayerInfo(n, &PlayerInfo))
			continue;

		m_vecSpectators.emplace_back(Spectator_t{ Utils::ConvertUtf8ToWide(PlayerInfo.name), nMode});
	}

	return !m_vecSpectators.empty();
}

void CSpectatorList::Drag()
{
	int nMouseX = H::Input->GetMouseX();
	int nMouseY = H::Input->GetMouseY();

	bool bHovered = false;
	static bool bDragging = false;

	if (!bDragging && F::Menu->IsMenuWindowHovered())
		return;

	static int nDeltaX = 0;
	static int nDeltaY = 0;

	int nListX = CFG::Visuals_SpectatorList_Pos_X;
	int nListY = CFG::Visuals_SpectatorList_Pos_Y;

	bHovered = nMouseX > nListX && nMouseX < nListX + LIST_WIDTH && nMouseY > nListY && nMouseY < nListY + CFG::Menu_Drag_Bar_Height;

	if (bHovered && H::Input->IsPressed(VK_LBUTTON)) {
		nDeltaX = nMouseX - nListX;
		nDeltaY = nMouseY - nListY;
		bDragging = true;
	}

	if (!H::Input->IsPressed(VK_LBUTTON) && !H::Input->IsHeld(VK_LBUTTON))
		bDragging = false;

	if (bDragging) {
		CFG::Visuals_SpectatorList_Pos_X = nMouseX - nDeltaX;
		CFG::Visuals_SpectatorList_Pos_Y = nMouseY - nDeltaY;
	}
}

void CSpectatorList::Run()
{
	if (!CFG::Visuals_SpectatorList_Active)
		return;

	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	if (!F::Menu->IsOpen() && (I::EngineVGui->IsGameUIVisible() || SDKUtils::BInEndOfMatch()))
		return;

	if (F::Menu->IsOpen())
		Drag();

	auto OutlineColor = []() -> Color_t {
		Color_t Out = CFG::Menu_Accent_Secondary;
		Out.a = static_cast<byte>(CFG::Visuals_SpectatorList_Outline_Alpha * 255.0f);
		return Out;
	}();

	auto BackgroundColor = []() -> Color_t {
		Color_t Out = CFG::Menu_Background;
		Out.a = static_cast<byte>(CFG::Visuals_SpectatorList_Background_Alpha * 255.0f);
		return Out;
	}();

	H::Draw->Rect(
		CFG::Visuals_SpectatorList_Pos_X,
		CFG::Visuals_SpectatorList_Pos_Y,
		LIST_WIDTH,
		CFG::Menu_Drag_Bar_Height,
		BackgroundColor
	);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		CFG::Visuals_SpectatorList_Pos_X + (LIST_WIDTH / 2),
		CFG::Visuals_SpectatorList_Pos_Y + (CFG::Menu_Drag_Bar_Height / 2),
		CFG::Menu_Text,
		POS_CENTERXY,
		"Spectators"
	);

	H::Draw->OutlinedRect(
		CFG::Visuals_SpectatorList_Pos_X,
		CFG::Visuals_SpectatorList_Pos_Y,
		LIST_WIDTH,
		CFG::Menu_Drag_Bar_Height,
		OutlineColor
	);

	if (!GetSpectators())
		return;

	for (size_t n = 0; n < m_vecSpectators.size(); n++)
	{
		const auto &Spectator = m_vecSpectators[n];

		H::Draw->Rect(
			CFG::Visuals_SpectatorList_Pos_X,
			CFG::Visuals_SpectatorList_Pos_Y + (CFG::Menu_Drag_Bar_Height * (n + 1)) - 1,
			LIST_WIDTH,
			CFG::Menu_Drag_Bar_Height + 1,
			BackgroundColor
		);

		int nModeX = CFG::Visuals_SpectatorList_Pos_X;
		int nModeOffsetX = LIST_WIDTH / 8;
		int nTextY = CFG::Visuals_SpectatorList_Pos_Y + (CFG::Menu_Drag_Bar_Height * (n + 1)) - 1;
		int nTextX = nModeX + nModeOffsetX + CFG::Menu_Spacing_X;
		int nY = CFG::Visuals_SpectatorList_Pos_Y + (CFG::Menu_Drag_Bar_Height * (n + 1)) - 1;

		H::Draw->Line(nModeX + nModeOffsetX, nY, nModeX + nModeOffsetX, nY + CFG::Menu_Drag_Bar_Height, OutlineColor);

		H::Draw->String(
			H::Fonts->Get(EFonts::Menu),
			nModeX + (nModeOffsetX / 2),
			nTextY + (CFG::Menu_Drag_Bar_Height / 2) + 1,
			CFG::Menu_Text_Inactive,
			POS_CENTERXY,
			Spectator.m_nMode == OBS_MODE_IN_EYE ? "1st" : "3rd"
		);

		I::MatSystemSurface->DisableClipping(false);
		I::MatSystemSurface->SetClippingRect(nModeX, nTextY, (nModeX + LIST_WIDTH) - (CFG::Menu_Spacing_X + 1), nTextY + CFG::Menu_Drag_Bar_Height);

		H::Draw->String(
			H::Fonts->Get(EFonts::Menu),
			nTextX,
			nTextY + (CFG::Menu_Drag_Bar_Height / 2) + 1,
			CFG::Menu_Text_Inactive,
			POS_CENTERY,
			Spectator.m_wsName.c_str()
		);

		I::MatSystemSurface->DisableClipping(true);

		H::Draw->OutlinedRect(
			CFG::Visuals_SpectatorList_Pos_X,
			CFG::Visuals_SpectatorList_Pos_Y + (CFG::Menu_Drag_Bar_Height * (n + 1)) - 1,
			LIST_WIDTH,
			CFG::Menu_Drag_Bar_Height + 1,
			OutlineColor
		);
	}
}