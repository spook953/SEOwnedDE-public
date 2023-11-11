#include "WorldModulation.h"

#include "../CFG.h"

void CWorldModulation::applyModulation(Color_t clr, bool world, bool sky)
{
	if (!world && !sky)
	{
		return;
	}

	for (auto h{ I::MaterialSystem->FirstMaterial() }; h != I::MaterialSystem->InvalidMaterial(); h = I::MaterialSystem->NextMaterial(h))
	{
		auto mat{ I::MaterialSystem->GetMaterial(h) };

		if (!mat || mat->IsErrorMaterial() || !mat->IsPrecached())
		{
			continue;
		}
		
		if (world && std::string_view(mat->GetTextureGroupName()).find(TEXTURE_GROUP_WORLD) != std::string_view::npos
			&& std::string_view(mat->GetName()).find("sky") == std::string_view::npos)
		{
			auto var{ mat->FindVar("$color2", nullptr) };

			if (var)
			{
				var->SetVecValue(ColorUtils::ToFloat(clr.r), ColorUtils::ToFloat(clr.g), ColorUtils::ToFloat(clr.b));
			}

			else
			{
				mat->ColorModulate(ColorUtils::ToFloat(clr.r), ColorUtils::ToFloat(clr.g), ColorUtils::ToFloat(clr.b));
			}
		}

		if (sky && std::string_view(mat->GetName()).find("sky") != std::string_view::npos)
		{
			mat->ColorModulate(ColorUtils::ToFloat(clr.r), ColorUtils::ToFloat(clr.g), ColorUtils::ToFloat(clr.b));
		}
	}

	m_bWorldWasModulated = true;
}

void CWorldModulation::UpdateWorldModulation()
{
	auto mode_changed = [&]()
	{
		static auto old{ CFG::Visuals_World_Modulation_Mode };

		auto cur{ CFG::Visuals_World_Modulation_Mode };

		if (cur != old)
		{
			old = cur;

			return true;
		}

		return false;
	};

	auto ignore_sky_changed = [&]()
	{
		static auto old{ CFG::Visuals_World_Modulation_No_Sky_Change };

		auto cur{ CFG::Visuals_World_Modulation_No_Sky_Change };

		if (cur != old)
		{
			old = cur;

			return true;
		}

		return false;
	};

	bool reset_sky{};

	if (ignore_sky_changed())
	{
		reset_sky = true;
	}

	if (CFG::Visuals_World_Modulation_Mode == 0)
	{
		if (CFG::Visuals_Night_Mode <= 0.0f)
		{
			RestoreWorldModulation();

			return;
		}

		auto value_changed = [&]()
		{
			static auto old{ CFG::Visuals_Night_Mode };

			auto cur{ CFG::Visuals_Night_Mode };

			if (cur != old)
			{
				old = cur;

				return true;
			}

			return false;
		};

		if (!m_bWorldWasModulated || value_changed() || mode_changed() || reset_sky)
		{
			auto col{ static_cast<byte>(Math::RemapValClamped(CFG::Visuals_Night_Mode, 0.0f, 100.0f, 255.0f, 5.0f)) };

			applyModulation({ col, col, col, static_cast<byte>(255) }, true, !CFG::Visuals_World_Modulation_No_Sky_Change);
		}

		if (reset_sky && CFG::Visuals_World_Modulation_No_Sky_Change)
		{
			applyModulation({ 255, 255, 255, 255 }, false, true);
		}
	}

	else
	{
		if (!m_bWorldWasModulated || mode_changed())
		{
			applyModulation(CFG::Color_World, true, false);
			applyModulation(CFG::Color_Sky, false, !CFG::Visuals_World_Modulation_No_Sky_Change);
		}

		auto world_value_changed = [&]()
		{
			static auto old{ CFG::Color_World };

			auto cur{ CFG::Color_World };

			if (cur.r != old.r || cur.g != old.g || cur.b != old.b)
			{
				old = cur;

				return true;
			}

			return false;
		};

		auto sky_value_changed = [&]()
		{
			static auto old{ CFG::Color_Sky };

			auto cur{ CFG::Color_Sky };

			if (cur.r != old.r || cur.g != old.g || cur.b != old.b)
			{
				old = cur;

				return true;
			}

			return false;
		};

		applyModulation(CFG::Color_World, world_value_changed(), false);
		
		if (!CFG::Visuals_World_Modulation_No_Sky_Change)
		{
			applyModulation(CFG::Color_Sky, false, sky_value_changed());
		}

		if (reset_sky)
		{
			if (CFG::Visuals_World_Modulation_No_Sky_Change)
			{
				applyModulation({ 255, 255, 255, 255 }, false, true);
			}

			else
			{
				applyModulation(CFG::Color_Sky, false, true);
			}
		}
	}
}

void CWorldModulation::RestoreWorldModulation()
{
	if (!m_bWorldWasModulated)
	{
		return;
	}

	applyModulation({ 255, 255, 255, 255 }, true, true);

	m_bWorldWasModulated = false;
}