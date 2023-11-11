#include "SpyWarning.h"

#include "../CFG.h"

#include "../VisualUtils/VisualUtils.h"
#include "../AimUtils/AimUtils.h"

#define SCREEN_OFFSET_X_SCALE 0.5f
#define SCREEN_OFFSET_Y_SCALE 0.2f
#define ARROW_RADIUS 32.0f
#define MAX_DIST 500.0f

void CSpyWarning::Run()
{
	if (!CFG::Viuals_SpyWarning_Active)
		return;

	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal || pLocal->deadflag())
		return;

	struct Player_t
	{
		C_TFPlayer *m_pPlayer{};
		Vec3 m_vPos{};
	};

	std::vector<Player_t> spies{};

	for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ENEMIES))
	{
		if (!pEntity)
			continue;

		auto pPlayer = pEntity->As<C_TFPlayer>();

		if (pPlayer->GetRenderCenter().DistTo(pLocal->GetRenderCenter()) > MAX_DIST)
			continue;

		if (!pPlayer || pPlayer->deadflag() || pPlayer->m_iClass() != TF_CLASS_SPY)
			continue;

		if (pPlayer->InCond(TF_COND_HALLOWEEN_KART) || pPlayer->InCond(TF_COND_HALLOWEEN_GHOST_MODE))
			continue;

		if (CFG::Viuals_SpyWarning_Ignore_Cloaked)
		{
			if (pPlayer->InCond(TF_COND_STEALTHED) || pPlayer->m_flInvisibility() >= 0.8f)
				continue;		
		}

		if (CFG::Viuals_SpyWarning_Ignore_Friends && pPlayer->IsPlayerOnSteamFriendsList())
			continue;

		if (Math::CalcFov(I::EngineClient->GetViewAngles(), Math::CalcAngle(pLocal->GetShootPos(), pPlayer->GetRenderCenter())) < 70.0f)
			continue;

		if (CFG::Viuals_SpyWarning_Ignore_Invisible)
		{
			if (!F::AimUtils->TracePositionWorld(pLocal->GetShootPos(), pPlayer->GetRenderCenter()))
				continue;
		}

		spies.push_back({ pPlayer, pPlayer->GetRenderCenter() });
	}

	static bool last_empty{ spies.empty() };

	if (spies.empty() != last_empty)
	{
		last_empty = spies.empty();

		if (!spies.empty() && CFG::Viuals_SpyWarning_Announce)
		{
			I::EngineClient->ClientCmd_Unrestricted("voicemenu 1 1");
		}
	}

	if (!I::EngineVGui->IsGameUIVisible())
	{
		for (const auto &spy : spies)
		{
			auto pPlayer = spy.m_pPlayer;

			if (!pPlayer)
				continue;

			int nScreenCenterX = static_cast<int>(static_cast<float>(H::Draw->GetScreenW()) * SCREEN_OFFSET_X_SCALE);
			int nScreenCenterY = static_cast<int>(static_cast<float>(H::Draw->GetScreenH()) * SCREEN_OFFSET_Y_SCALE);

			Vec3 vScreen = {};
			H::Draw->ScreenPosition(spy.m_vPos, vScreen);

			Vec3 vAngle = {};
			Math::VectorAngles({ nScreenCenterX - vScreen.x, nScreenCenterY - vScreen.y, 0.0f }, vAngle);

			float flYaw = DEG2RAD(vAngle.y);

			float flRadius = Math::RemapValClamped(pLocal->GetShootPos().DistTo(spy.m_vPos), 0.0f, MAX_DIST, ARROW_RADIUS, ARROW_RADIUS * 2.0f);
			float flScale = Math::RemapValClamped(pLocal->GetShootPos().DistTo(spy.m_vPos), 0.0f, MAX_DIST, 2.0f, 1.0f);

			float flDrawX = nScreenCenterX - flRadius * cosf(flYaw);
			float flDrawY = nScreenCenterY - flRadius * sinf(flYaw);

			std::array<Vec2, 3> vPoints = {
				Vec2(flDrawX + (6.0f * flScale), flDrawY + (6.0f * flScale)),
				Vec2(flDrawX - (4.0f * flScale), flDrawY),
				Vec2(flDrawX + (6.0f * flScale), flDrawY - (6.0f * flScale))
			};

			Math::RotateTriangle(vPoints, vAngle.y);

			H::Draw->FilledTriangle(vPoints, F::VisualUtils->GetEntityColor(pLocal, pPlayer));
		}

		if (!spies.empty())
		{
			int nScreenCenterX = static_cast<int>(static_cast<float>(H::Draw->GetScreenW()) * SCREEN_OFFSET_X_SCALE);
			int nScreenCenterY = static_cast<int>(static_cast<float>(H::Draw->GetScreenH()) * SCREEN_OFFSET_Y_SCALE);

			H::Draw->Texture(nScreenCenterX, nScreenCenterY, 36, 36, F::VisualUtils->GetClassIcon(TF_CLASS_SPY), POS_CENTERXY);
		}
	}
}