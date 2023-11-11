#include "Materials.h"

#include "../CFG.h"
#include "../VisualUtils/VisualUtils.h"
#include "../LagRecords/LagRecords.h"
#include "../AimUtils/AimUtils.h"

void SetModelStencilForOutlines(C_BaseEntity *pEntity)
{
	IMatRenderContext *pRenderContext = I::MaterialSystem->GetRenderContext();

	if (!pRenderContext)
		return;

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal)
		return;

	auto IsEntGoingToBeGlowed = [&]()
	{
		if (pEntity->GetClassId() == ETFClassIds::CTFPlayer)
		{
			if (!CFG::Outlines_Players_Active)
				return false;

			auto pPlayer = pEntity->As<C_TFPlayer>();

			bool bIsLocal = pPlayer == pLocal;
			bool bIsFriend = pPlayer->IsPlayerOnSteamFriendsList();

			if (CFG::Outlines_Players_Ignore_Local && bIsLocal)
				return false;

			if (CFG::Outlines_Players_Ignore_Friends && bIsFriend)
				return false;

			if (!bIsLocal)
			{
				if (!bIsFriend)
				{
					if (CFG::Outlines_Players_Ignore_Teammates && pPlayer->m_iTeamNum() == pLocal->m_iTeamNum())
					{
						if (CFG::Outlines_Players_Show_Teammate_Medics)
						{
							if (pPlayer->m_iClass() != TF_CLASS_MEDIC)
								return false;
						}

						else return false;
					}

					if (CFG::Outlines_Players_Ignore_Enemies && pPlayer->m_iTeamNum() != pLocal->m_iTeamNum())
						return false;
				}
			}
		}

		if (pEntity->GetClassId() == ETFClassIds::CObjectSentrygun
			|| pEntity->GetClassId() == ETFClassIds::CObjectDispenser
			|| pEntity->GetClassId() == ETFClassIds::CObjectTeleporter)
		{
			if (!CFG::Outlines_Buildings_Active)
				return false;

			auto pBuilding = pEntity->As<C_BaseObject>();

			if (pBuilding->m_bPlacing())
				return false;

			bool bIsLocal = F::VisualUtils->IsEntityOwnedBy(pBuilding, pLocal);

			if (CFG::Outlines_Buildings_Ignore_Local && bIsLocal)
				return false;

			if (!bIsLocal)
			{
				if (CFG::Outlines_Buildings_Ignore_Teammates && pBuilding->m_iTeamNum() == pLocal->m_iTeamNum())
				{
					if (CFG::Outlines_Buildings_Show_Teammate_Dispensers)
					{
						if (pBuilding->GetClassId() != ETFClassIds::CObjectDispenser)
							return false;
					}

					else return false;
				}

				if (CFG::Outlines_Buildings_Ignore_Enemies && pBuilding->m_iTeamNum() != pLocal->m_iTeamNum())
					return false;
			}
		}

		//fuck rest

		return true;
	};

	if (!IsEntGoingToBeGlowed())
		pRenderContext->SetStencilEnable(false);

	else
	{
		ShaderStencilState_t State = {};
		State.m_bEnable = true;
		State.m_nReferenceValue = 1;
		State.m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
		State.m_PassOp = STENCILOPERATION_REPLACE;
		State.m_FailOp = STENCILOPERATION_KEEP;
		State.m_ZFailOp = STENCILOPERATION_REPLACE;
		State.SetStencilState(pRenderContext);
	}
}

void CMaterials::Initialize()
{
	static ConVar *mat_hdr_level = I::CVar->FindVar("mat_hdr_level");

	if (!m_pFlat)
	{
		auto *kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$dummy", "dummy");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumFresnel", "1");
		kv->SetString("$selfillumFresnelMinMaxExp", "[0.4999 0.5 1]");
		kv->SetString("$cloakPassEnabled", "1");
		kv->SetString("$nodecal", "1");
		kv->SetString("$model", "1");
		if (auto proxies = kv->FindKey("Proxies", true)) { proxies->FindKey("invis", true); }
		m_pFlat = I::MaterialSystem->CreateMaterial("seo_material_flat", kv);
	}

	if (!m_pShaded)
	{
		auto *kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumFresnel", "1");
		kv->SetString("$selfillumFresnelMinMaxExp", "[0.1 0.5 2]");
		kv->SetString("$cloakPassEnabled", "1");
		kv->SetString("$nodecal", "1");
		kv->SetString("$model", "1");
		if (auto proxies = kv->FindKey("Proxies", true)) { proxies->FindKey("invis", true); }
		m_pShaded = I::MaterialSystem->CreateMaterial("seo_material_shaded", kv);
	}

	if (!m_pGlossy)
	{
		auto *kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen002");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelranges", "[0 1 2]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumFresnel", "1");
		kv->SetString("$selfillumFresnelMinMaxExp", "[0 0.1 1]");
		kv->SetString("$cloakPassEnabled", "1");
		kv->SetString("$nodecal", "1");
		kv->SetString("$model", "1");
		if (auto proxies = kv->FindKey("Proxies", true)) { proxies->FindKey("invis", true); }
		m_pGlossy = I::MaterialSystem->CreateMaterial("seo_material_glossy", kv);
	}

	if (!m_pGlow)
	{
		auto *kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$envmap", "effects/saxxy_gold");
		//kv->SetString("$envmap", "skybox/sky_dustbowl_01");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelranges", mat_hdr_level->GetInt() > 1 ? "[0 0.05 0.1]" : "[0 1 2]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumFresnel", "1");
		kv->SetString("$selfillumFresnelMinMaxExp", "[0.4999 0.5 0]");
		kv->SetString("$envmaptint", "[0 0 0]");
		kv->SetString("$selfillumtint", "[0.03 0.03 0.03]");
		kv->SetString("$cloakPassEnabled", "1");
		kv->SetString("$nodecal", "1");
		kv->SetString("$model", "1");
		if (auto proxies = kv->FindKey("Proxies", true)) { proxies->FindKey("invis", true); }
		m_pGlow = I::MaterialSystem->CreateMaterial("seo_material_glow", kv);
		m_pGlowEnvmapTint = m_pGlow->FindVar("$envmaptint", nullptr);
		m_pGlowSelfillumTint = m_pGlow->FindVar("$selfillumtint", nullptr);
	}

	if (!m_pPlastic)
	{
		auto *kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "models/player/shared/ice_player");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$phong", "1");
		kv->SetString("$phongexponent", "10");
		kv->SetString("$phongboost", "1");
		kv->SetString("$phongfresnelranges", "[0 0 0]");
		kv->SetString("$basemapalphaphongmask", "1");
		kv->SetString("$phongwarptexture", "models/player/shared/ice_player_warp");
		kv->SetString("$cloakPassEnabled", "1");
		kv->SetString("$nodecal", "1");
		kv->SetString("$model", "1");
		if (auto proxies = kv->FindKey("Proxies", true)) { proxies->FindKey("invis", true); }
		m_pPlastic = I::MaterialSystem->CreateMaterial("seo_material_plastic", kv);
	}

	if (!m_pFlatNoInvis)
	{
		auto *kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumFresnel", "1");
		kv->SetString("$selfillumFresnelMinMaxExp", "[0.4999 0.5 1]");
		kv->SetString("$nodecal", "1");
		kv->SetString("$model", "1");
		m_pFlatNoInvis = I::MaterialSystem->CreateMaterial("seo_material_flat_no_invis", kv);
	}

	if (!m_pShadedNoInvis)
	{
		auto *kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumFresnel", "1");
		kv->SetString("$selfillumFresnelMinMaxExp", "[0.1 0.5 2]");
		kv->SetString("$nodecal", "1");
		kv->SetString("$model", "1");
		m_pShadedNoInvis = I::MaterialSystem->CreateMaterial("seo_material_shaded_no_invis", kv);
	}
}

void CMaterials::DrawEntity(C_BaseEntity *pEntity)
{
	SetModelStencilForOutlines(pEntity);

	m_bRendering = true;

	float flOldInvisibility = pEntity->GetClassId() == ETFClassIds::CTFPlayer ? pEntity->As<C_TFPlayer>()->m_flInvisibility() : -1.0f;

	if (flOldInvisibility > 0.99f) {
		pEntity->As<C_TFPlayer>()->m_flInvisibility() = 0.0f;
		I::RenderView->SetBlend(0.0f);
	}

	pEntity->DrawModel(STUDIO_RENDER);

	if (flOldInvisibility > 0.99f) {
		pEntity->As<C_TFPlayer>()->m_flInvisibility() = flOldInvisibility;
		I::RenderView->SetBlend(1.0f);
	}

	m_mapDrawnEntities[pEntity] = true;

	m_bRendering = false;
}

void CMaterials::RunLagRecords()
{
	auto pRenderContext = I::MaterialSystem->GetRenderContext();

	if (!pRenderContext || !CFG::Materials_Players_Active || CFG::Materials_Players_Ignore_LagRecords)
		return;

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal)
		return;

	auto pWeapon = H::Entities->GetWeapon();

	if (!pWeapon)
		return;

	auto WeaponType = F::AimUtils->GetWeaponType(pWeapon);

	if (WeaponType == EWeaponType::HITSCAN)
	{
		if (!CFG::Aimbot_Active
			|| !CFG::Aimbot_Hitscan_Active
			|| !CFG::Aimbot_Target_Players
			|| !CFG::Aimbot_Hitscan_Target_LagRecords)
			return;
	}

	else if (WeaponType == EWeaponType::MELEE)
	{
		if (!CFG::Aimbot_Active
			|| !CFG::Aimbot_Melee_Active
			|| !CFG::Aimbot_Target_Players
			|| !CFG::Aimbot_Melee_Target_LagRecords)
			return;
	}

	else return;

	m_bRenderingOriginalMat = false;

	I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

	I::ModelRender->ForcedMaterialOverride(CFG::Materials_Players_LagRecords_Style == 0 ? m_pFlatNoInvis : m_pShadedNoInvis);

	if (CFG::Materials_Players_No_Depth)
		pRenderContext->DepthRange(0.0f, 0.2f);

	for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ENEMIES))
	{
		if (!pEntity)
			continue;

		auto pPlayer = pEntity->As<C_TFPlayer>();

		if (pPlayer->deadflag())
			continue;

		int nRecords = 0;

		if (!F::LagRecords->HasRecords(pPlayer, &nRecords))
			continue;

		if (nRecords <= 0)
			continue;

		if (CFG::Materials_Players_LagRecords_Style == 0)
		{
			for (int n = 1; n < nRecords; n++)
			{
				auto pRecord = F::LagRecords->GetRecord(pPlayer, n, true);

				if (!pRecord || !F::VisualUtils->IsOnScreenNoEntity(pLocal, pRecord->m_vAbsOrigin) || !F::LagRecords->DiffersFromCurrent(pRecord))
					continue;

				I::RenderView->SetBlend(Math::RemapValClamped(static_cast<float>(n), 1.0f, static_cast<float>(nRecords), 0.1f, 0.001f));

				F::LagRecordMatrixHelper->Set(pRecord);
				m_bRendering = true;
				float flOldInvisibility = pPlayer->m_flInvisibility();
				pPlayer->m_flInvisibility() = 0.0f;
				pPlayer->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);
				pPlayer->m_flInvisibility() = flOldInvisibility;
				m_bRendering = false;
				F::LagRecordMatrixHelper->Restore();
			}
		}

		else
		{
			auto pRecord = F::LagRecords->GetRecord(pPlayer, nRecords - 1, true);

			if (!pRecord || !F::VisualUtils->IsOnScreenNoEntity(pLocal, pRecord->m_vAbsOrigin) || !F::LagRecords->DiffersFromCurrent(pRecord))
				continue;

			I::RenderView->SetBlend(1.0f);

			F::LagRecordMatrixHelper->Set(pRecord);
			m_bRendering = true;
			float flOldInvisibility = pPlayer->m_flInvisibility();
			pPlayer->m_flInvisibility() = 0.0f;
			pPlayer->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);
			pPlayer->m_flInvisibility() = flOldInvisibility;
			m_bRendering = false;
			F::LagRecordMatrixHelper->Restore();
		}
	}

	I::ModelRender->ForcedMaterialOverride(nullptr);

	if (CFG::Materials_Players_No_Depth)
		pRenderContext->DepthRange(0.0f, 1.0f);

	I::RenderView->SetBlend(1.0f);
}

void CMaterials::Run()
{
	Initialize();

	if (!m_mapDrawnEntities.empty())
		m_mapDrawnEntities.clear();

	if (!CFG::Materials_Active || I::EngineVGui->IsGameUIVisible() || G::bRenderingSpyCamera)
		return;

	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	auto pRenderContext = I::MaterialSystem->GetRenderContext();

	if (!pRenderContext)
		return;

	auto pLocal = H::Entities->GetLocal();

	if (!pLocal)
		return;

	m_pGlowSelfillumTint->SetVecValue(0.03f, 0.03f, 0.03f);

	RunLagRecords();

	auto GetMaterial = [&](int nIndex) -> IMaterial *
	{
		//don't forget to change me if more materials are added!
		m_bRenderingOriginalMat = nIndex == 0 || nIndex > 5;

		switch (nIndex)
		{
			case 0: return nullptr;
			case 1: return m_pFlat;
			case 2: return m_pShaded;
			case 3: return m_pGlossy;
			case 4: return m_pGlow;
			case 5: return m_pPlastic;
			default: return nullptr;
		}
	};

	if (CFG::Materials_Players_Active)
	{
		I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

		auto pMaterial = GetMaterial(CFG::Materials_Players_Material);

		if (pMaterial)
			I::ModelRender->ForcedMaterialOverride(pMaterial);

		if (CFG::Materials_Players_Alpha < 1.0f)
			I::RenderView->SetBlend(CFG::Materials_Players_Alpha);

		if (CFG::Materials_Players_No_Depth)
			pRenderContext->DepthRange(0.0f, 0.2f);

		for (auto pEntity : H::Entities->GetGroup(EEntGroup::PLAYERS_ALL))
		{
			if (!pEntity)
				continue;

			auto pPlayer = pEntity->As<C_TFPlayer>();

			if (pPlayer->deadflag())
				continue;

			bool bIsLocal = pPlayer == pLocal;
			bool bIsFriend = pPlayer->IsPlayerOnSteamFriendsList();

			if (CFG::Materials_Players_Ignore_Local && bIsLocal)
				continue;

			if (CFG::Materials_Players_Ignore_Friends && bIsFriend)
				continue;

			if (!bIsLocal)
			{
				if (!bIsFriend)
				{
					if (CFG::Materials_Players_Ignore_Teammates && pPlayer->m_iTeamNum() == pLocal->m_iTeamNum())
					{
						if (CFG::Materials_Players_Show_Teammate_Medics)
						{
							if (pPlayer->m_iClass() != TF_CLASS_MEDIC)
								continue;
						}

						else continue;
					}

					if (CFG::Materials_Players_Ignore_Enemies && pPlayer->m_iTeamNum() != pLocal->m_iTeamNum())
						continue;
				}
			}

			if (!F::VisualUtils->IsOnScreen(pLocal, pPlayer))
				continue;

			auto Color = F::VisualUtils->GetEntityColor(pLocal, pPlayer);

			if (pMaterial && pMaterial != m_pGlow)
				I::RenderView->SetColorModulation(Color);

			if (pMaterial == m_pGlow)
				m_pGlowEnvmapTint->SetVecValue(ColorUtils::ToFloat(Color.r), ColorUtils::ToFloat(Color.g), ColorUtils::ToFloat(Color.b));

			DrawEntity(pPlayer);

			C_BaseEntity *pAttach = pPlayer->FirstMoveChild();

			for (int n = 0; n < 32; n++)
			{
				if (!pAttach)
					break;

				if (pAttach->ShouldDraw())
					DrawEntity(pAttach);

				pAttach = pAttach->NextMovePeer();
			}
		}

		if (pMaterial)
			I::ModelRender->ForcedMaterialOverride(nullptr);

		if (CFG::Materials_Players_Alpha < 1.0f)
			I::RenderView->SetBlend(1.0f);

		if (CFG::Materials_Players_No_Depth)
			pRenderContext->DepthRange(0.0f, 1.0f);
	}

	if (CFG::Materials_Buildings_Active)
	{
		I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

		auto pMaterial = GetMaterial(CFG::Materials_Buildings_Material);

		if (pMaterial)
			I::ModelRender->ForcedMaterialOverride(pMaterial);

		if (CFG::Materials_Buildings_Alpha < 1.0f)
			I::RenderView->SetBlend(CFG::Materials_Buildings_Alpha);

		if (CFG::Materials_Buildings_No_Depth)
			pRenderContext->DepthRange(0.0f, 0.2f);

		for (auto pEntity : H::Entities->GetGroup(EEntGroup::BUILDINGS_ALL))
		{
			if (!pEntity)
				continue;

			auto pBuilding = pEntity->As<C_BaseObject>();

			if (pBuilding->m_bPlacing())
				continue;

			bool bIsLocal = F::VisualUtils->IsEntityOwnedBy(pBuilding, pLocal);

			if (CFG::Materials_Buildings_Ignore_Local && bIsLocal)
				continue;

			if (!bIsLocal)
			{
				if (CFG::Materials_Buildings_Ignore_Teammates && pBuilding->m_iTeamNum() == pLocal->m_iTeamNum())
				{
					if (CFG::Materials_Buildings_Show_Teammate_Dispensers)
					{
						if (pBuilding->GetClassId() != ETFClassIds::CObjectDispenser)
							continue;
					}

					else continue;
				}

				if (CFG::Materials_Buildings_Ignore_Enemies && pBuilding->m_iTeamNum() != pLocal->m_iTeamNum())
					continue;
			}

			if (!F::VisualUtils->IsOnScreen(pLocal, pBuilding))
				continue;

			auto Color = F::VisualUtils->GetEntityColor(pLocal, pBuilding);

			if (pMaterial && pMaterial != m_pGlow)
				I::RenderView->SetColorModulation(Color);

			if (pMaterial == m_pGlow)
				m_pGlowEnvmapTint->SetVecValue(ColorUtils::ToFloat(Color.r), ColorUtils::ToFloat(Color.g), ColorUtils::ToFloat(Color.b));

			DrawEntity(pBuilding);
		}

		if (pMaterial)
			I::ModelRender->ForcedMaterialOverride(nullptr);

		if (CFG::Materials_Buildings_Alpha < 1.0f)
			I::RenderView->SetBlend(1.0f);

		if (CFG::Materials_Buildings_No_Depth)
			pRenderContext->DepthRange(0.0f, 1.0f);
	}

	if (CFG::Materials_World_Active)
	{
		I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

		auto pMaterial = GetMaterial(CFG::Materials_World_Material);

		if (pMaterial)
			I::ModelRender->ForcedMaterialOverride(pMaterial);

		if (CFG::Materials_World_Alpha < 1.0f)
			I::RenderView->SetBlend(CFG::Materials_World_Alpha);

		if (CFG::Materials_World_No_Depth)
			pRenderContext->DepthRange(0.0f, 0.2f);

		if (!CFG::Materials_World_Ignore_HealthPacks)
		{
			auto Color = CFG::Color_HealthPack;

			for (auto pEntity : H::Entities->GetGroup(EEntGroup::HEALTHPACKS))
			{
				if (!pEntity || !F::VisualUtils->IsOnScreen(pLocal, pEntity))
					continue;

				if (pMaterial && pMaterial != m_pGlow)
					I::RenderView->SetColorModulation(Color);

				if (pMaterial == m_pGlow)
					m_pGlowEnvmapTint->SetVecValue(ColorUtils::ToFloat(Color.r), ColorUtils::ToFloat(Color.g), ColorUtils::ToFloat(Color.b));

				DrawEntity(pEntity);
			}
		}

		if (!CFG::Materials_World_Ignore_AmmoPacks)
		{
			auto Color = CFG::Color_AmmoPack;

			for (auto pEntity : H::Entities->GetGroup(EEntGroup::AMMOPACKS))
			{
				if (!pEntity || !F::VisualUtils->IsOnScreen(pLocal, pEntity))
					continue;

				if (pMaterial && pMaterial != m_pGlow)
					I::RenderView->SetColorModulation(Color);

				if (pMaterial == m_pGlow)
					m_pGlowEnvmapTint->SetVecValue(ColorUtils::ToFloat(Color.r), ColorUtils::ToFloat(Color.g), ColorUtils::ToFloat(Color.b));

				DrawEntity(pEntity);
			}
		}

		if (!CFG::Materials_World_Ignore_Halloween_Gift)
		{
			auto Color = CFG::Color_Halloween_Gift;

			for (auto pEntity : H::Entities->GetGroup(EEntGroup::HALLOWEEN_GIFT))
			{
				if (!pEntity || !pEntity->ShouldDraw() || !F::VisualUtils->IsOnScreen(pLocal, pEntity))
					continue;

				if (pMaterial && pMaterial != m_pGlow)
					I::RenderView->SetColorModulation(Color);

				if (pMaterial == m_pGlow)
					m_pGlowEnvmapTint->SetVecValue(ColorUtils::ToFloat(Color.r), ColorUtils::ToFloat(Color.g), ColorUtils::ToFloat(Color.b));

				DrawEntity(pEntity);
			}
		}

		if (!CFG::Materials_World_Ignore_MVM_Money)
		{
			auto Color = CFG::Color_MVM_Money;

			for (auto pEntity : H::Entities->GetGroup(EEntGroup::MVM_MONEY))
			{
				if (!pEntity || !pEntity->ShouldDraw() || !F::VisualUtils->IsOnScreen(pLocal, pEntity))
					continue;

				if (pMaterial && pMaterial != m_pGlow)
					I::RenderView->SetColorModulation(Color);

				if (pMaterial == m_pGlow)
					m_pGlowEnvmapTint->SetVecValue(ColorUtils::ToFloat(Color.r), ColorUtils::ToFloat(Color.g), ColorUtils::ToFloat(Color.b));

				DrawEntity(pEntity);
			}
		}

		bool bIgnoringAllProjectiles = CFG::Materials_World_Ignore_LocalProjectiles
			&& CFG::Materials_World_Ignore_EnemyProjectiles
			&& CFG::Materials_World_Ignore_TeammateProjectiles;

		if (!bIgnoringAllProjectiles)
		{
			for (auto pEntity : H::Entities->GetGroup(EEntGroup::PROJECTILES_ALL))
			{
				if (!pEntity || !pEntity->ShouldDraw())
					continue;

				bool bIsLocal = F::VisualUtils->IsEntityOwnedBy(pEntity, pLocal);

				if (CFG::Materials_World_Ignore_LocalProjectiles && bIsLocal)
					continue;

				if (!bIsLocal)
				{
					if (CFG::Materials_World_Ignore_EnemyProjectiles && pEntity->m_iTeamNum() != pLocal->m_iTeamNum())
						continue;

					if (CFG::Materials_World_Ignore_TeammateProjectiles && pEntity->m_iTeamNum() == pLocal->m_iTeamNum())
						continue;
				}

				if (!F::VisualUtils->IsOnScreen(pLocal, pEntity))
					continue;

				auto Color = F::VisualUtils->GetEntityColor(pLocal, pEntity);

				if (pMaterial && pMaterial != m_pGlow)
					I::RenderView->SetColorModulation(Color);

				if (pMaterial == m_pGlow)
					m_pGlowEnvmapTint->SetVecValue(ColorUtils::ToFloat(Color.r), ColorUtils::ToFloat(Color.g), ColorUtils::ToFloat(Color.b));

				DrawEntity(pEntity);
			}
		}

		if (pMaterial)
			I::ModelRender->ForcedMaterialOverride(nullptr);

		if (CFG::Materials_World_Alpha < 1.0f)
			I::RenderView->SetBlend(1.0f);

		if (CFG::Materials_World_No_Depth)
			pRenderContext->DepthRange(0.0f, 1.0f);
	}
}

void CMaterials::CleanUp()
{
	m_bCleaningUp = true;

	if (m_pFlat)
	{
		m_pFlat->DecrementReferenceCount();
		m_pFlat->DeleteIfUnreferenced();
		m_pFlat = nullptr;
	}

	if (m_pShaded)
	{
		m_pShaded->DecrementReferenceCount();
		m_pShaded->DeleteIfUnreferenced();
		m_pShaded = nullptr;
	}

	if (m_pGlossy)
	{
		m_pGlossy->DecrementReferenceCount();
		m_pGlossy->DeleteIfUnreferenced();
		m_pGlossy = nullptr;
	}

	if (m_pGlow)
	{
		m_pGlow->DecrementReferenceCount();
		m_pGlow->DeleteIfUnreferenced();
		m_pGlow = nullptr;
		m_pGlowEnvmapTint = nullptr;
		m_pGlowSelfillumTint = nullptr;
	}

	if (m_pPlastic)
	{
		m_pPlastic->DecrementReferenceCount();
		m_pPlastic->DeleteIfUnreferenced();
		m_pPlastic = nullptr;
	}

	if (m_pFlatNoInvis)
	{
		m_pFlatNoInvis->DecrementReferenceCount();
		m_pFlatNoInvis->DeleteIfUnreferenced();
		m_pFlatNoInvis = nullptr;
	}

	if (m_pShadedNoInvis)
	{
		m_pShadedNoInvis->DecrementReferenceCount();
		m_pShadedNoInvis->DeleteIfUnreferenced();
		m_pShadedNoInvis = nullptr;
	}

	m_bCleaningUp = false;
}