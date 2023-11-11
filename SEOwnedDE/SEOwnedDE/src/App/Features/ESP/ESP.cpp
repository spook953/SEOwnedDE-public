#include "ESP.h"

#include "../CFG.h"
#include "../VisualUtils/VisualUtils.h"

constexpr int SPACING_X = 2;
constexpr int SPACING_Y = 2;
constexpr Color_t WHITE = { 220, 220, 220, 255 };

bool CESP::GetDrawBounds(C_BaseEntity *pEntity, int &x, int &y, int &w, int &h)
{
	if (!pEntity)
		return false;

	bool bIsPlayer = false;
	Vec3 vMins = {}, vMaxs = {};
	matrix3x4_t &Transform = const_cast<matrix3x4_t &>(pEntity->RenderableToWorldTransform());

	switch (pEntity->GetClassId())
	{
		case ETFClassIds::CTFPlayer:
		{
			auto pPlayer = pEntity->As<C_TFPlayer>();

			bIsPlayer = true;
			vMins = pPlayer->m_vecMins();
			vMaxs = pPlayer->m_vecMaxs();

			if (pPlayer->entindex() == I::EngineClient->GetLocalPlayer())
			{
				Vec3 vAngles = I::EngineClient->GetViewAngles();
				vAngles.x = vAngles.z = 0.0f;
				Math::AngleMatrix(vAngles, Transform);
				Math::MatrixSetColumn(pPlayer->GetRenderOrigin(), 3, Transform);
			}

			break;
		}

		case ETFClassIds::CObjectSentrygun:
		case ETFClassIds::CObjectDispenser:
		case ETFClassIds::CObjectTeleporter: {
			vMins = pEntity->m_vecMins();
			vMaxs = pEntity->m_vecMaxs();
			break;
		}

		default: {
			pEntity->GetRenderBounds(vMins, vMaxs);
			break;
		}
	}

	Vec3 vPoints[] =
	{
		Vec3(vMins.x, vMins.y, vMins.z),
		Vec3(vMins.x, vMaxs.y, vMins.z),
		Vec3(vMaxs.x, vMaxs.y, vMins.z),
		Vec3(vMaxs.x, vMins.y, vMins.z),
		Vec3(vMaxs.x, vMaxs.y, vMaxs.z),
		Vec3(vMins.x, vMaxs.y, vMaxs.z),
		Vec3(vMins.x, vMins.y, vMaxs.z),
		Vec3(vMaxs.x, vMins.y, vMaxs.z)
	};

	Vec3 vTransformed[8] = {};

	for (int n = 0; n < 8; n++)
		Math::VectorTransform(vPoints[n], Transform, vTransformed[n]);

	Vec3 flb = {}, brt = {}, blb = {}, frt = {}, frb = {}, brb = {}, blt = {}, flt = {};

	if (H::Draw->W2S(vTransformed[3], flb) && H::Draw->W2S(vTransformed[5], brt)
		&& H::Draw->W2S(vTransformed[0], blb) && H::Draw->W2S(vTransformed[4], frt)
		&& H::Draw->W2S(vTransformed[2], frb) && H::Draw->W2S(vTransformed[1], brb)
		&& H::Draw->W2S(vTransformed[6], blt) && H::Draw->W2S(vTransformed[7], flt)
		&& H::Draw->W2S(vTransformed[6], blt) && H::Draw->W2S(vTransformed[7], flt))
	{
		Vec3 arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

		float left = flb.x;
		float top = flb.y;
		float righ = flb.x;
		float bottom = flb.y;

		for (int n = 1; n < 8; n++)
		{
			if (left > arr[n].x)
				left = arr[n].x;

			if (top < arr[n].y)
				top = arr[n].y;

			if (righ < arr[n].x)
				righ = arr[n].x;

			if (bottom > arr[n].y)
				bottom = arr[n].y;
		}

		float x_ = left;
		float y_ = bottom;
		float w_ = (righ - left);
		float h_ = (top - bottom);

		if (bIsPlayer) {
			x_ += ((righ - left) / 8.0f);
			w_ -= (((righ - left) / 8.0f) * 2.0f);
		}

		x = static_cast<int>(x_);
		y = static_cast<int>(y_);
		w = static_cast<int>(w_);
		h = static_cast<int>(h_);

		return x <= H::Draw->GetScreenW() && (x + w) >= 0 && y <= H::Draw->GetScreenH() && (y + h) >= 0;
	}

	return false;
}

void CESP::DrawBones(C_TFPlayer *pPlayer, Color_t Color)
{
	auto MatrixPosition = [](const matrix3x4_t &matrix, Vector &position)
	{
		position[0] = matrix[0][3];
		position[1] = matrix[1][3];
		position[2] = matrix[2][3];
	};

	const model_t *pModel = pPlayer->GetModel();

	if (!pModel)
		return;

	const studiohdr_t *pStudioHdr = I::ModelInfoClient->GetStudiomodel(pModel);

	if (!pStudioHdr)
		return;

	matrix3x4_t Matrix[MAXSTUDIOBONES];

	if (!pPlayer->SetupBones(Matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, I::GlobalVars->curtime))
		return;

	Vec3 p1 = {}, p2 = {};
	Vec3 p1s = {}, p2s = {};

	for (int n = 0; n < pStudioHdr->numbones; n++)
	{
		const mstudiobone_t *pBone = pStudioHdr->pBone(n);

		if (!pBone || pBone->parent == -1 || !(pBone->flags & BONE_USED_BY_HITBOX))
			continue;

		MatrixPosition(Matrix[n], p1);

		if (!H::Draw->W2S(p1, p1s))
			continue;

		MatrixPosition(Matrix[pBone->parent], p2);

		if (!H::Draw->W2S(p2, p2s))
			continue;

		H::Draw->Line(static_cast<int>(p1s.x), static_cast<int>(p1s.y), static_cast<int>(p2s.x), static_cast<int>(p2s.y), Color);
	}
}

void CESP::Run()
{
	if (!CFG::ESP_Active || I::EngineVGui->IsGameUIVisible() || SDKUtils::BInEndOfMatch() || G::bRenderingSpyCamera)
		return;

	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal)
		return;

	float flOriginalAlpha = I::MatSystemSurface->DrawGetAlphaMultiplier();

	if (CFG::ESP_Players_Active)
	{
		I::MatSystemSurface->DrawSetAlphaMultiplier(CFG::ESP_Players_Alpha);

		for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ALL))
		{
			if (!pEntity)
				continue;

			auto pPlayer = pEntity->As<C_TFPlayer>();

			if (pPlayer->deadflag())
				continue;

			bool bIsLocal = pPlayer == pLocal;
			bool bIsFriend = pPlayer->IsPlayerOnSteamFriendsList();

			if ((CFG::ESP_Players_Ignore_Local && bIsLocal) || (!I::Input->CAM_IsThirdPerson() && bIsLocal))
				continue;

			if (CFG::ESP_Players_Ignore_Friends && bIsFriend)
				continue;
			
			if (!bIsLocal)
			{
				if (!bIsFriend)
				{
					if (CFG::ESP_Players_Ignore_Teammates && pPlayer->m_iTeamNum() == pLocal->m_iTeamNum())
					{
						if (CFG::ESP_Players_Show_Teammate_Medics)
						{
							if (pPlayer->m_iClass() != TF_CLASS_MEDIC)
								continue;
						}

						else continue;
					}

					if (CFG::ESP_Players_Ignore_Enemies && pPlayer->m_iTeamNum() != pLocal->m_iTeamNum())
						continue;
				}

				if (CFG::ESP_Players_Ignore_Invisible && pPlayer->m_flInvisibility() >= 1.0f)
					continue;
			}

			int x = 0, y = 0, w = 0, h = 0;

			if (!GetDrawBounds(pPlayer, x, y, w, h))
			{
				if (CFG::ESP_Players_Arrows && pPlayer != pLocal && !pLocal->deadflag())
				{
					int nScreenCenterX = H::Draw->GetScreenW() / 2;
					int nScreenCenterY = H::Draw->GetScreenH() / 2;

					Vec3 vScreen = {}, vPlayerWSC = pPlayer->GetCenter();
					H::Draw->ScreenPosition(vPlayerWSC, vScreen);

					if (pLocal->GetCenter().DistTo(vPlayerWSC) > CFG::ESP_Players_Arrows_Max_Distance)
						continue;

					Vec3 vAngle = {};
					Math::VectorAngles({ nScreenCenterX - vScreen.x, nScreenCenterY - vScreen.y, 0.0f }, vAngle);

					float flYaw = DEG2RAD(vAngle.y);
					float flRadius = CFG::ESP_Players_Arrows_Radius;
					float flDrawX = nScreenCenterX - flRadius * cosf(flYaw);
					float flDrawY = nScreenCenterY - flRadius * sinf(flYaw);

					std::array<Vec2, 3> vPoints = {
						Vec2(flDrawX + 10.0f, flDrawY + 10.0f),
						Vec2(flDrawX - 8.0f, flDrawY),
						Vec2(flDrawX + 10.0f, flDrawY - 10.0f)
					};

					Math::RotateTriangle(vPoints, vAngle.y);

					H::Draw->FilledTriangle(vPoints, F::VisualUtils->GetEntityColor(pLocal, pPlayer));
				}

				continue;
			}

			auto Color = F::VisualUtils->GetEntityColor(pLocal, pPlayer);
			auto HealthColor = F::VisualUtils->GetHealthColor(pPlayer->m_iHealth(), pPlayer->GetMaxHealth());
			auto TextColor = CFG::ESP_Text_Color == 0 ? Color : CFG::Color_ESP_Text;
			
			int nTextOffsetY = 0;

			if (CFG::ESP_Players_Tracer)
			{
				if (!bIsLocal)
				{
					auto nFromY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_From)
						{
							case 0: return 0;
							case 1: return H::Draw->GetScreenH() / 2;
							case 2: return H::Draw->GetScreenH();
							default: return 0;
						}
					};

					auto nToY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_To)
						{
							case 0: return y;
							case 1: return y + (h / 2);
							case 2: return y + h;
							default: return 0;
						}
					};

					H::Draw->Line(H::Draw->GetScreenW() / 2, nFromY(), x + (w / 2), nToY(), Color);
				}
			}

			if (CFG::ESP_Players_Bones)
			{
				DrawBones(pPlayer, CFG::ESP_Players_Bones_Color == 0 ? Color : WHITE);
			}

			if (CFG::ESP_Players_Name)
			{
				player_info_t PlayerInfo = {};

				if (I::EngineClient->GetPlayerInfo(pPlayer->entindex(), &PlayerInfo))
				{
					H::Draw->String(
						H::Fonts->Get(EFonts::ESP_SMALL),
						x + (w / 2),
						(y - (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall - 1)) - SPACING_Y,
						TextColor,
						POS_CENTERX,
						Utils::ConvertUtf8ToWide(PlayerInfo.name).c_str()
					);
				}
			}

			if (CFG::ESP_Players_Class)
			{
				auto ClassStr = [&]() -> const char *
				{
					switch (pPlayer->m_iClass())
					{
						case TF_CLASS_SCOUT: return "Scout";
						case TF_CLASS_SOLDIER: return "Soldier";
						case TF_CLASS_PYRO: return "Pyro";
						case TF_CLASS_DEMOMAN: return "Demoman";
						case TF_CLASS_HEAVYWEAPONS: return "Heavy";
						case TF_CLASS_ENGINEER: return "Engineer";
						case TF_CLASS_MEDIC: return "Medic";
						case TF_CLASS_SNIPER: return "Sniper";
						case TF_CLASS_SPY: return "Spy";
						default: return "Unknown Class";
					}
				};

				H::Draw->String(
					H::Fonts->Get(EFonts::ESP_SMALL),
					x + w + SPACING_X,
					y + (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall * nTextOffsetY++),
					TextColor,
					POS_DEFAULT,
					"%hs",
					ClassStr()
				);
			}

			if (CFG::ESP_Players_Class_Icon)
			{
				static const int nClassIconSize = 18;

				H::Draw->Texture(
					x + (w / 2),
					CFG::ESP_Players_Name ? ((y - (H::Fonts->Get(EFonts::ESP).m_nTall - 1)) - SPACING_Y) - nClassIconSize : y - (nClassIconSize + SPACING_Y),
					nClassIconSize,
					nClassIconSize,
					F::VisualUtils->GetClassIcon(pPlayer->m_iClass()),
					POS_CENTERX
				);
			}

			if (CFG::ESP_Players_Health && !pPlayer->InCond(TF_COND_HALLOWEEN_GHOST_MODE))
			{
				H::Draw->String(
					H::Fonts->Get(EFonts::ESP_SMALL),
					x + w + SPACING_X,
					y + (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall * nTextOffsetY++),
					HealthColor,
					POS_DEFAULT,
					"%d",
					pPlayer->m_iHealth()
				);
			}

			if (CFG::ESP_Players_HealthBar && !pPlayer->InCond(TF_COND_HALLOWEEN_GHOST_MODE))
			{
				float flHealth = static_cast<float>(pPlayer->m_iHealth());
				float flMaxHealth = static_cast<float>(pPlayer->GetMaxHealth());

				if (flHealth && flMaxHealth)
				{
					if (flHealth > flMaxHealth)
						flMaxHealth = flHealth;

					static const int nBarWidth = 2;
					int nBarX = x - ((nBarWidth * 2) + 1);
					int nFillH = static_cast<int>(Math::RemapValClamped(flHealth, 0.0f, flMaxHealth, 0.0f, static_cast<float>(h)));

					H::Draw->OutlinedRect(nBarX - 1, (y + h - nFillH) - 1, nBarWidth + 2, nFillH + 2, CFG::Color_ESP_Outline);
					H::Draw->Rect(nBarX, y + h - nFillH, nBarWidth, nFillH, HealthColor);
				}
			}

			if (CFG::ESP_Players_Uber)
			{
				if (pPlayer->m_iClass() == TF_CLASS_MEDIC)
				{
					if (auto pWeapon = pPlayer->GetWeaponFromSlot(1))
					{
						H::Draw->String(
							H::Fonts->Get(EFonts::ESP_SMALL),
							x + w + SPACING_X,
							y + (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall * nTextOffsetY++),
							CFG::Color_Uber,
							POS_DEFAULT,
							"%d%%", static_cast<int>(pWeapon->As<C_WeaponMedigun>()->m_flChargeLevel() * 100.0f)
						);
					}
				}
			}

			if (CFG::ESP_Players_UberBar)
			{
				if (pPlayer->m_iClass() == TF_CLASS_MEDIC)
				{
					if (auto pWeapon = pPlayer->GetWeaponFromSlot(1))
					{
						auto pMedigun = pWeapon->As<C_WeaponMedigun>();

						if (auto flCharge = pMedigun->m_flChargeLevel())
						{
							int nBarH = 2;
							int nDrawY = y + h + nBarH + 1;
							float flFillW = Math::RemapValClamped(flCharge, 0.0f, 1.0f, 0.0f, static_cast<float>(w));

							H::Draw->OutlinedRect(x - 1, nDrawY - 1, static_cast<int>(flFillW) + 2, nBarH + 2, CFG::Color_ESP_Outline);
							H::Draw->Rect(x, nDrawY, static_cast<int>(flFillW), nBarH, CFG::Color_Uber);

							if (pMedigun->m_iItemDefinitionIndex() == Medic_s_TheVaccinator)
							{
								if (flCharge >= 0.25f)
									H::Draw->Rect(x + static_cast<int>(static_cast<float>(w) * 0.25f) - 1, nDrawY, 2, nBarH, CFG::Color_ESP_Outline);

								if (flCharge >= 0.5f)
									H::Draw->Rect(x + static_cast<int>(static_cast<float>(w) * 0.5f) - 1, nDrawY, 2, nBarH, CFG::Color_ESP_Outline);

								if (flCharge >= 0.75f)
									H::Draw->Rect(x + static_cast<int>(static_cast<float>(w) * 0.75f) - 1, nDrawY, 2, nBarH, CFG::Color_ESP_Outline);
							}
						}
					}
				}
			}

			if (CFG::ESP_Players_Box)
			{
				H::Draw->OutlinedRect(x, y, w, h, Color);
				H::Draw->OutlinedRect(x - 1, y - 1, w + 2, h + 2, CFG::Color_ESP_Outline);
			}

			if (CFG::ESP_Players_Conds)
			{
				if (nTextOffsetY > 0)
					nTextOffsetY += 1;

				int draw_x = x + w + SPACING_X;
				int tall = H::Fonts->Get(EFonts::ESP_CONDS).m_nTall;

				Color_t Color = CFG::Color_Conds;

				if (pPlayer->IsZoomed())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "ZOOM");

				if (pPlayer->IsInvisible())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "INVIS");

				if (pPlayer->m_bFeignDeathReady())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "DEADRINGER");

				if (pPlayer->IsInvulnerable())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "INVULN");

				if (pPlayer->IsCritBoosted())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "CRIT");

				if (pPlayer->IsMiniCritBoosted())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "MINICRIT");

				if (pPlayer->IsMarked())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "MARKED");

				if (pPlayer->InCond(TF_COND_MAD_MILK))
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "MILK");

				if (pPlayer->InCond(TF_COND_TAUNTING) || (pPlayer == pLocal && G::bStartedFakeTaunt))
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "TAUNT");

				if (pPlayer->InCond(TF_COND_DISGUISED))
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "DISGUISE");

				if (pPlayer->InCond(TF_COND_BURNING) || pPlayer->InCond(TF_COND_BURNING_PYRO))
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "BURNING");

				if (pPlayer->InCond(TF_COND_OFFENSEBUFF))
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "BANNER");

				if (pPlayer->InCond(TF_COND_DEFENSEBUFF))
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "BACKUP");

				if (pPlayer->InCond(TF_COND_REGENONDAMAGEBUFF))
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "CONCH");

				if (!pPlayer->InCond(TF_COND_MEDIGUN_UBER_BULLET_RESIST))
				{
					if (pPlayer->InCond(TF_COND_MEDIGUN_SMALL_BULLET_RESIST))
						H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "BULLET(RES)");
				}

				else H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "BULLET(UBER)");

				if (!pPlayer->InCond(TF_COND_MEDIGUN_UBER_BLAST_RESIST))
				{
					if (pPlayer->InCond(TF_COND_MEDIGUN_SMALL_BLAST_RESIST))
						H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "EXPLOSION(RES)");
				}

				else H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "EXPLOSION(UBER)");

				if (!pPlayer->InCond(TF_COND_MEDIGUN_UBER_FIRE_RESIST))
				{
					if (pPlayer->InCond(TF_COND_MEDIGUN_SMALL_FIRE_RESIST))
						H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "FIRE(RES)");
				}

				else H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "FIRE(UBER)");
			}
		}
	}

	if (CFG::ESP_Buildings_Active)
	{
		I::MatSystemSurface->DrawSetAlphaMultiplier(CFG::ESP_Buildings_Alpha);

		for (auto pEntity : H::Entities->GetGroup(EEntGroup::BUILDINGS_ALL))
		{
			if (!pEntity)
				continue;

			auto pBuilding = pEntity->As<C_BaseObject>();

			if (pBuilding->m_bPlacing())
				continue;

			bool bIsLocal = F::VisualUtils->IsEntityOwnedBy(pBuilding, pLocal);

			if (CFG::ESP_Buildings_Ignore_Local && bIsLocal)
				continue;

			if (!bIsLocal)
			{		
				if (CFG::ESP_Buildings_Ignore_Teammates && pBuilding->m_iTeamNum() == pLocal->m_iTeamNum())
				{
					if (CFG::ESP_Buildings_Show_Teammate_Dispensers)
					{
						if (pBuilding->GetClassId() != ETFClassIds::CObjectDispenser)
							continue;
					}

					else continue;
				}

				if (CFG::ESP_Buildings_Ignore_Enemies && pBuilding->m_iTeamNum() != pLocal->m_iTeamNum())
					continue;
			}

			int x = 0, y = 0, w = 0, h = 0;

			if (!GetDrawBounds(pBuilding, x, y, w, h))
				continue;

			auto Color = F::VisualUtils->GetEntityColor(pLocal, pBuilding);
			auto HealthColor = F::VisualUtils->GetHealthColor(pBuilding->m_iHealth(), pBuilding->m_iMaxHealth());
			auto TextColor = CFG::ESP_Text_Color == 0 ? Color : CFG::Color_ESP_Text;

			int nTextOffsetY = 0;

			if (CFG::ESP_Buildings_Tracer)
			{
				auto nFromY = [&]() -> int
				{
					switch (CFG::ESP_Tracer_From)
					{
						case 0: return 0;
						case 1: return H::Draw->GetScreenH() / 2;
						case 2: return H::Draw->GetScreenH();
						default: return 0;
					}
				};

				auto nToY = [&]() -> int
				{
					switch (CFG::ESP_Tracer_To)
					{
						case 0: return y;
						case 1: return y + (h / 2);
						case 2: return y + h;
						default: return 0;
					}
				};

				H::Draw->Line(H::Draw->GetScreenW() / 2, nFromY(), x + (w / 2), nToY(), Color);
			}

			if (CFG::ESP_Buildings_Name)
			{
				auto Name = [&]() -> const char *
				{
					switch (pBuilding->GetClassId())
					{
						case ETFClassIds::CObjectSentrygun: return pBuilding->m_bMiniBuilding() ? "Mini Sentrygun" : "Sentrygun";
						case ETFClassIds::CObjectDispenser: return "Dispenser";
						case ETFClassIds::CObjectTeleporter: return pBuilding->m_iObjectMode() == MODE_TELEPORTER_ENTRANCE ? "Teleporter In" : "Teleporter Out";
						default: return "Unknown Building Name";
					}
				};

				H::Draw->String(
					H::Fonts->Get(EFonts::ESP_SMALL),
					x + (w / 2),
					(y - (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall - 1)) - SPACING_Y,
					TextColor,
					POS_CENTERX,
					Name()
				);
			}

			if (CFG::ESP_Buildings_Health)
			{
				H::Draw->String(
					H::Fonts->Get(EFonts::ESP_SMALL),
					x + w + SPACING_X,
					y + (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall * nTextOffsetY++),
					HealthColor,
					POS_DEFAULT,
					"%d",
					pBuilding->m_iHealth()
				);
			}

			if (CFG::ESP_Buildings_HealthBar)
			{
				float flHealth = static_cast<float>(pBuilding->m_iHealth());
				float flMaxHealth = static_cast<float>(pBuilding->m_iMaxHealth());

				if (flHealth && flMaxHealth)
				{
					if (flHealth > flMaxHealth)
						flMaxHealth = flHealth;

					static const int nBarWidth = 2;
					int nBarX = x - ((nBarWidth * 2) + 1);
					int nFillH = static_cast<int>(Math::RemapValClamped(flHealth, 0.0f, flMaxHealth, 0.0f, static_cast<float>(h)));

					H::Draw->OutlinedRect(nBarX - 1, (y + h - nFillH) - 1, nBarWidth + 2, nFillH + 2, CFG::Color_ESP_Outline);
					H::Draw->Rect(nBarX, y + h - nFillH, nBarWidth, nFillH, HealthColor);
				}
			}

			if (CFG::ESP_Buildings_Level)
			{
				H::Draw->String(
					H::Fonts->Get(EFonts::ESP_SMALL),
					x + w + SPACING_X,
					y + (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall * nTextOffsetY++),
					TextColor,
					POS_DEFAULT,
					"%d",
					pBuilding->m_iUpgradeLevel()
				);
			}

			if (CFG::ESP_Buildings_LevelBar)
			{
				int nBarH = 2;
				int nDrawY = y + h + nBarH + 1;
				float flLevel = static_cast<float>(pBuilding->m_iUpgradeLevel());
				float flFillW = Math::RemapValClamped(flLevel, 1.0f, 3.0f, static_cast<float>(w / 3.0f), static_cast<float>(w));

				if (pBuilding->m_bMiniBuilding())
					flFillW = static_cast<float>(w);

				H::Draw->OutlinedRect(x - 1, nDrawY - 1, static_cast<int>(flFillW) + 2, nBarH + 2, CFG::Color_ESP_Outline);
				H::Draw->Rect(x, nDrawY, static_cast<int>(flFillW), nBarH, WHITE);
			}

			if (CFG::ESP_Buildings_Box)
			{
				H::Draw->OutlinedRect(x, y, w, h, Color);
				H::Draw->OutlinedRect(x - 1, y - 1, w + 2, h + 2, CFG::Color_ESP_Outline);
			}
			
			if (CFG::ESP_Buildings_Conds)
			{
				if (nTextOffsetY > 0)
					nTextOffsetY += 1;

				int draw_x = x + w + SPACING_X;
				int tall = H::Fonts->Get(EFonts::ESP_CONDS).m_nTall;

				Color_t Color = CFG::Color_Conds;

				if (pBuilding->m_bBuilding())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "BUILDING");

				else
				{
					if (pBuilding->GetClassId() == ETFClassIds::CObjectSentrygun && pBuilding->As<C_ObjectSentrygun>()->m_iAmmoShells() == 0)
						H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "NOSHELLS");

					if (!pBuilding->m_bMiniBuilding())
					{
						if (pBuilding->GetClassId() == ETFClassIds::CObjectSentrygun && pBuilding->As<C_ObjectSentrygun>()->m_iAmmoRockets() == 0)
						{
							if (pBuilding->m_iUpgradeLevel() == 3)
								H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "NOROCKETS");
						}
					}
				}

				if (pBuilding->IsDisabled())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "DISABLED");

				if (pBuilding->GetClassId() == ETFClassIds::CObjectSentrygun && pBuilding->As<C_ObjectSentrygun>()->m_bShielded())
					H::Draw->String(H::Fonts->Get(EFonts::ESP_CONDS), draw_x, y + (tall * nTextOffsetY++), Color, POS_DEFAULT, "WRANGLED");
			}
		}
	}

	if (CFG::ESP_World_Active)
	{
		I::MatSystemSurface->DrawSetAlphaMultiplier(CFG::ESP_World_Alpha);

		int x = 0, y = 0, w = 0, h = 0;

		if (!CFG::ESP_World_Ignore_HealthPacks)
		{
			auto Color = CFG::Color_HealthPack;
			auto TextColor = CFG::ESP_Text_Color == 0 ? Color : CFG::Color_ESP_Text;

			for (auto pEntity : H::Entities->GetGroup(EEntGroup::HEALTHPACKS))
			{
				if (!pEntity || !GetDrawBounds(pEntity, x, y, w, h))
					continue;

				if (CFG::ESP_World_Tracer)
				{
					auto nFromY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_From)
						{
							case 0: return 0;
							case 1: return H::Draw->GetScreenH() / 2;
							case 2: return H::Draw->GetScreenH();
							default: return 0;
						}
					};

					auto nToY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_To)
						{
							case 0: return y;
							case 1: return y + (h / 2);
							case 2: return y + h;
							default: return 0;
						}
					};

					H::Draw->Line(H::Draw->GetScreenW() / 2, nFromY(), x + (w / 2), nToY(), Color);
				}

				if (CFG::ESP_World_Name)
				{
					H::Draw->String(
						H::Fonts->Get(EFonts::ESP_SMALL),
						x + (w / 2),
						(y - (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall - 1)) - SPACING_Y,
						TextColor,
						POS_CENTERX,
						"health"
					);
				}

				if (CFG::ESP_World_Box)
				{
					H::Draw->OutlinedRect(x, y, w, h, Color);
					H::Draw->OutlinedRect(x - 1, y - 1, w + 2, h + 2, CFG::Color_ESP_Outline);
				}
			}
		}

		if (!CFG::ESP_World_Ignore_AmmoPacks)
		{
			auto Color = CFG::Color_AmmoPack;
			auto TextColor = CFG::ESP_Text_Color == 0 ? Color : CFG::Color_ESP_Text;

			for (auto pEntity : H::Entities->GetGroup(EEntGroup::AMMOPACKS))
			{
				if (!pEntity || !GetDrawBounds(pEntity, x, y, w, h))
					continue;

				if (CFG::ESP_World_Tracer)
				{
					auto nFromY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_From)
						{
							case 0: return 0;
							case 1: return H::Draw->GetScreenH() / 2;
							case 2: return H::Draw->GetScreenH();
							default: return 0;
						}
					};

					auto nToY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_To)
						{
							case 0: return y;
							case 1: return y + (h / 2);
							case 2: return y + h;
							default: return 0;
						}
					};

					H::Draw->Line(H::Draw->GetScreenW() / 2, nFromY(), x + (w / 2), nToY(), Color);
				}

				if (CFG::ESP_World_Name)
				{
					H::Draw->String(
						H::Fonts->Get(EFonts::ESP_SMALL),
						x + (w / 2),
						(y - (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall - 1)) - SPACING_Y,
						TextColor,
						POS_CENTERX,
						"ammo"
					);
				}

				if (CFG::ESP_World_Box)
				{
					H::Draw->OutlinedRect(x, y, w, h, Color);
					H::Draw->OutlinedRect(x - 1, y - 1, w + 2, h + 2, CFG::Color_ESP_Outline);
				}
			}
		}

		bool bIgnoringAllProjectiles = CFG::ESP_World_Ignore_LocalProjectiles
			&& CFG::ESP_World_Ignore_EnemyProjectiles
			&& CFG::ESP_World_Ignore_TeammateProjectiles;

		if (!bIgnoringAllProjectiles)
		{
			for (auto pEntity : H::Entities->GetGroup(EEntGroup::PROJECTILES_ALL))
			{
				if (!pEntity || !pEntity->ShouldDraw())
					continue;

				bool bIsLocal = F::VisualUtils->IsEntityOwnedBy(pEntity, pLocal);

				if (CFG::ESP_World_Ignore_LocalProjectiles && bIsLocal)
					continue;

				if (!bIsLocal)
				{
					if (CFG::ESP_World_Ignore_EnemyProjectiles && pEntity->m_iTeamNum() != pLocal->m_iTeamNum())
						continue;

					if (CFG::ESP_World_Ignore_TeammateProjectiles && pEntity->m_iTeamNum() == pLocal->m_iTeamNum())
						continue;
				}

				if (!GetDrawBounds(pEntity, x, y, w, h))
					continue;

				auto Color = F::VisualUtils->GetEntityColor(pLocal, pEntity);
				auto TextColor = CFG::ESP_Text_Color == 0 ? Color : CFG::Color_ESP_Text;

				if (CFG::ESP_World_Tracer)
				{
					auto nFromY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_From)
						{
							case 0: return 0;
							case 1: return H::Draw->GetScreenH() / 2;
							case 2: return H::Draw->GetScreenH();
							default: return 0;
						}
					};

					auto nToY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_To)
						{
							case 0: return y;
							case 1: return y + (h / 2);
							case 2: return y + h;
							default: return 0;
						}
					};

					H::Draw->Line(H::Draw->GetScreenW() / 2, nFromY(), x + (w / 2), nToY(), Color);
				}

				if (CFG::ESP_World_Name)
				{
					auto Name = [&]() -> const char *
					{
						switch (pEntity->GetClassId())
						{
							case ETFClassIds::CTFProjectile_Rocket:
							case ETFClassIds::CTFProjectile_SentryRocket: return "Rocket";
							case ETFClassIds::CTFProjectile_Jar: return "Jarate";
							case ETFClassIds::CTFProjectile_JarGas: return "Gas";
							case ETFClassIds::CTFProjectile_JarMilk: return "Milk";
							case ETFClassIds::CTFProjectile_Arrow: return "Arrow";
							case ETFClassIds::CTFProjectile_Flare: return "Flare";
							case ETFClassIds::CTFProjectile_Cleaver: return "Cleaver";
							case ETFClassIds::CTFProjectile_HealingBolt: return "Healing Arrow";
							case ETFClassIds::CTFGrenadePipebombProjectile:
							{
								auto pPipebomb = pEntity->As<C_TFGrenadePipebombProjectile>();

								return pPipebomb->HasStickyEffects() ? "Sticky" : pPipebomb->m_iType() == TF_GL_MODE_CANNONBALL ? "Cannonball" : "Pipe";
							}

							default: return "Projectile";
						}
					};

					H::Draw->String(
						H::Fonts->Get(EFonts::ESP_SMALL),
						x + (w / 2),
						(y - (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall - 1)) - SPACING_Y,
						TextColor,
						POS_CENTERX,
						Name()
					);
				}

				if (CFG::ESP_World_Box)
				{
					H::Draw->OutlinedRect(x, y, w, h, Color);
					H::Draw->OutlinedRect(x - 1, y - 1, w + 2, h + 2, CFG::Color_ESP_Outline);
				}
			}
		}

		if (!CFG::ESP_World_Ignore_Halloween_Gift)
		{
			auto Color = CFG::Color_Halloween_Gift;
			auto TextColor = CFG::ESP_Text_Color == 0 ? Color : CFG::Color_ESP_Text;

			for (auto pEntity : H::Entities->GetGroup(EEntGroup::HALLOWEEN_GIFT))
			{
				if (!pEntity || !pEntity->ShouldDraw() || !GetDrawBounds(pEntity, x, y, w, h))
					continue;

				if (CFG::ESP_World_Tracer)
				{
					auto nFromY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_From)
						{
							case 0: return 0;
							case 1: return H::Draw->GetScreenH() / 2;
							case 2: return H::Draw->GetScreenH();
							default: return 0;
						}
					};

					auto nToY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_To)
						{
							case 0: return y;
							case 1: return y + (h / 2);
							case 2: return y + h;
							default: return 0;
						}
					};

					H::Draw->Line(H::Draw->GetScreenW() / 2, nFromY(), x + (w / 2), nToY(), Color);
				}

				if (CFG::ESP_World_Name)
				{
					H::Draw->String(
						H::Fonts->Get(EFonts::ESP_SMALL),
						x + (w / 2),
						(y - (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall - 1)) - SPACING_Y,
						TextColor,
						POS_CENTERX,
						"halloween gift"
					);
				}

				if (CFG::ESP_World_Box)
				{
					H::Draw->OutlinedRect(x, y, w, h, Color);
					H::Draw->OutlinedRect(x - 1, y - 1, w + 2, h + 2, CFG::Color_ESP_Outline);
				}
			}
		}

		if (!CFG::ESP_World_Ignore_Halloween_Gift)
		{
			auto Color = CFG::Color_MVM_Money;
			auto TextColor = CFG::ESP_Text_Color == 0 ? Color : CFG::Color_ESP_Text;

			for (auto pEntity : H::Entities->GetGroup(EEntGroup::MVM_MONEY))
			{
				if (!pEntity || !pEntity->ShouldDraw() || !GetDrawBounds(pEntity, x, y, w, h))
					continue;

				if (CFG::ESP_World_Tracer)
				{
					auto nFromY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_From)
						{
							case 0: return 0;
							case 1: return H::Draw->GetScreenH() / 2;
							case 2: return H::Draw->GetScreenH();
							default: return 0;
						}
					};

					auto nToY = [&]() -> int
					{
						switch (CFG::ESP_Tracer_To)
						{
							case 0: return y;
							case 1: return y + (h / 2);
							case 2: return y + h;
							default: return 0;
						}
					};

					H::Draw->Line(H::Draw->GetScreenW() / 2, nFromY(), x + (w / 2), nToY(), Color);
				}

				if (CFG::ESP_World_Name)
				{
					H::Draw->String(
						H::Fonts->Get(EFonts::ESP_SMALL),
						x + (w / 2),
						(y - (H::Fonts->Get(EFonts::ESP_SMALL).m_nTall - 1)) - SPACING_Y,
						TextColor,
						POS_CENTERX,
						"money"
					);
				}

				if (CFG::ESP_World_Box)
				{
					H::Draw->OutlinedRect(x, y, w, h, Color);
					H::Draw->OutlinedRect(x - 1, y - 1, w + 2, h + 2, CFG::Color_ESP_Outline);
				}
			}
		}
	}

	I::MatSystemSurface->DrawSetAlphaMultiplier(flOriginalAlpha);
}