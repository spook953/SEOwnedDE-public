#include "TeamWellBeing.h"

#include "../CFG.h"
#include "../Menu/Menu.h"
#include "../VisualUtils/VisualUtils.h"

void CTeamWellBeing::Drag()
{
	int nMouseX = H::Input->GetMouseX();
	int nMouseY = H::Input->GetMouseY();

	bool bHovered = false;
	static bool bDragging = false;

	if (!bDragging && F::Menu->IsMenuWindowHovered())
		return;

	static int nDeltaX = 0;
	static int nDeltaY = 0;

	int nListX = CFG::Visuals_TeamWellBeing_Pos_X;
	int nListY = CFG::Visuals_TeamWellBeing_Pos_Y;

	bHovered = nMouseX > nListX && nMouseX < nListX + CFG::Visuals_TeamWellBeing_Width && nMouseY > nListY && nMouseY < nListY + CFG::Menu_Drag_Bar_Height;

	if (bHovered && H::Input->IsPressed(VK_LBUTTON)) {
		nDeltaX = nMouseX - nListX;
		nDeltaY = nMouseY - nListY;
		bDragging = true;
	}

	if (!H::Input->IsPressed(VK_LBUTTON) && !H::Input->IsHeld(VK_LBUTTON))
		bDragging = false;

	if (bDragging) {
		CFG::Visuals_TeamWellBeing_Pos_X = nMouseX - nDeltaX;
		CFG::Visuals_TeamWellBeing_Pos_Y = nMouseY - nDeltaY;
	}
}

void CTeamWellBeing::Run()
{
	if (!CFG::Visuals_TeamWellBeing_Active)
		return;

	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	if (!F::Menu->IsOpen() && (I::EngineVGui->IsGameUIVisible() || SDKUtils::BInEndOfMatch()))
		return;

	if (F::Menu->IsOpen())
		Drag();

	else
	{
		if (CFG::Visuals_TeamWellBeing_Medic_Only)
		{
			auto local{ H::Entities->GetLocal() };

			if (local && local->m_iClass() != TF_CLASS_MEDIC)
			{
				return;
			}
		}
	}

	auto OutlineColor = []() -> Color_t {
		Color_t Out = CFG::Menu_Accent_Secondary;
		//Out.a = static_cast<byte>(CFG::Visuals_TeamWellBeing_Outline_Alpha * 255.0f);
		return Out;
	}();

	auto BackgroundColor = []() -> Color_t {
		Color_t Out = CFG::Menu_Background;
		Out.a = static_cast<byte>(CFG::Visuals_TeamWellBeing_Background_Alpha * 255.0f);
		return Out;
	}();

	H::Draw->Rect(
		CFG::Visuals_TeamWellBeing_Pos_X,
		CFG::Visuals_TeamWellBeing_Pos_Y,
		CFG::Visuals_TeamWellBeing_Width,
		CFG::Menu_Drag_Bar_Height,
		BackgroundColor
	);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		CFG::Visuals_TeamWellBeing_Pos_X + (CFG::Visuals_TeamWellBeing_Width / 2),
		CFG::Visuals_TeamWellBeing_Pos_Y + (CFG::Menu_Drag_Bar_Height / 2),
		CFG::Menu_Text,
		POS_CENTERXY,
		"Team Well-Being"
	);

	H::Draw->OutlinedRect(
		CFG::Visuals_TeamWellBeing_Pos_X,
		CFG::Visuals_TeamWellBeing_Pos_Y,
		CFG::Visuals_TeamWellBeing_Width,
		CFG::Menu_Drag_Bar_Height,
		OutlineColor
	);

	auto local{ H::Entities->GetLocal() };

	if (!local || local->deadflag() || (CFG::Visuals_TeamWellBeing_Medic_Only && local->m_iClass() != TF_CLASS_MEDIC))
	{
		return;
	}

	auto drawn{ 1 };

	for (auto ent : H::Entities->GetGroup(EEntGroup::PLAYERS_TEAMMATES))
	{
		if (!ent || ent == local)
		{
			continue;
		}

		auto player{ ent->As<C_TFPlayer>() };

		if (player->deadflag() || player->GetCenter().DistTo(local->GetShootPos()) > 449.0f)
		{
			continue;
		}

		player_info_t pi{};

		if (!I::EngineClient->GetPlayerInfo(player->entindex(), &pi))
		{
			continue;
		}

		auto draw_y{ (CFG::Visuals_TeamWellBeing_Pos_Y + (CFG::Menu_Drag_Bar_Height * drawn)) - 1 };
		auto text_y{ draw_y + (CFG::Menu_Drag_Bar_Height / 2) };

		auto name_separator_x{ CFG::Visuals_TeamWellBeing_Pos_X
			+ static_cast<int>(static_cast<float>(CFG::Visuals_TeamWellBeing_Width) * 0.3f) };

		auto class_separator_x{ CFG::Visuals_TeamWellBeing_Pos_X
			+ static_cast<int>(static_cast<float>(CFG::Visuals_TeamWellBeing_Width) * 0.3f) + 18 };

		H::Draw->Rect(
			CFG::Visuals_TeamWellBeing_Pos_X,
			draw_y,
			CFG::Visuals_TeamWellBeing_Width,
			CFG::Menu_Drag_Bar_Height + 1,
			BackgroundColor
		);

		H::Draw->StartClipping
		(
			CFG::Visuals_TeamWellBeing_Pos_X,
			draw_y,
			static_cast<int>(static_cast<float>(CFG::Visuals_TeamWellBeing_Width) * 0.3f) - CFG::Menu_Spacing_X,
			CFG::Menu_Drag_Bar_Height
		);

		H::Draw->String(
			H::Fonts->Get(EFonts::Menu),
			CFG::Visuals_TeamWellBeing_Pos_X + CFG::Menu_Spacing_X,
			text_y,
			CFG::Menu_Text_Inactive,
			POS_CENTERY,
			Utils::ConvertUtf8ToWide(pi.name).c_str()
		);

		H::Draw->EndClipping();

		H::Draw->Texture(class_separator_x - (18 / 2), text_y, 13, 13, F::VisualUtils->GetClassIcon(player->m_iClass()), POS_CENTERXY);

		auto bar_start_x{ class_separator_x + CFG::Menu_Spacing_X };
		auto max_bar_w{ static_cast<int>(static_cast<float>(CFG::Visuals_TeamWellBeing_Width) * 0.7f) - (18 + (CFG::Menu_Spacing_X * 2)) };
		auto bar_h{ CFG::Menu_Drag_Bar_Height - (CFG::Menu_Spacing_Y * 2) + 1 };
		auto bar_w{ static_cast<int>(Math::RemapValClamped(static_cast<float>(player->m_iHealth()), 0.0f, static_cast<float>(player->GetMaxHealth()), 0.0f, static_cast<float>(max_bar_w))) };
		auto overheal_bar_w{ static_cast<int>(Math::RemapValClamped(static_cast<float>(player->m_iHealth()), static_cast<float>(player->GetMaxHealth()), static_cast<float>(player->GetMaxHealth()) * 1.45f, 0.0f, static_cast<float>(max_bar_w))) };

		H::Draw->Rect
		(
			bar_start_x,
			draw_y + CFG::Menu_Spacing_Y,
			max_bar_w,
			bar_h,
			{ 0, 0, 0, 200 }
		);

		if (bar_w)
		{
			H::Draw->Rect
			(
				bar_start_x,
				draw_y + CFG::Menu_Spacing_Y,
				bar_w,
				bar_h,
				F::VisualUtils->GetHealthColorAlt(player->m_iHealth(), player->GetMaxHealth())
			);

			if (overheal_bar_w)
			{
				H::Draw->Rect
				(
					bar_start_x,
					draw_y + CFG::Menu_Spacing_Y,
					overheal_bar_w,
					bar_h,
					CFG::Color_OverHeal
				);
			}

			H::Draw->OutlinedRect
			(
				bar_start_x,
				draw_y + CFG::Menu_Spacing_Y,
				max_bar_w,
				bar_h,
				{ 0, 0, 0, 253 }
			);
		}

		H::Draw->OutlinedRect(
			CFG::Visuals_TeamWellBeing_Pos_X,
			draw_y,
			CFG::Visuals_TeamWellBeing_Width,
			CFG::Menu_Drag_Bar_Height + 1,
			OutlineColor
		);

		H::Draw->OutlinedRect(
			name_separator_x,
			draw_y,
			1,
			CFG::Menu_Drag_Bar_Height + 1,
			OutlineColor
		);

		H::Draw->OutlinedRect(
			class_separator_x,
			draw_y,
			1,
			CFG::Menu_Drag_Bar_Height + 1,
			OutlineColor
		);

		drawn++;
	}
}