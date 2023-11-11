#include "Entities.h"
#include "../../TF2/icliententitylist.h"
#include "../../TF2/ivmodelinfo.h"
#include "../../TF2/c_baseentity.h"

C_TFPlayer *CEntityHelper::GetLocal()
{
	if (auto pEntity = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer()))
		return pEntity->As<C_TFPlayer>();

	return nullptr;
}

C_TFWeaponBase *CEntityHelper::GetWeapon()
{
	if (auto pLocal = GetLocal())
	{
		if (auto pEntity = pLocal->m_hActiveWeapon().Get())
			return pEntity->As<C_TFWeaponBase>();
	}

	return nullptr;
}

void CEntityHelper::UpdateCache()
{
	if (auto pLocal = GetLocal())
	{
		int nLocalTeam = 0;

		if (!pLocal->IsInValidTeam(&nLocalTeam))
			return;

		for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
		{
			IClientEntity *pClientEntity = I::ClientEntityList->GetClientEntity(n);

			if (!pClientEntity || pClientEntity->IsDormant())
				continue;

			auto pEntity = pClientEntity->As<C_BaseEntity>();

			switch (pEntity->GetClassId())
			{
				case ETFClassIds::CTFPlayer:
				{
					int nPlayerTeam = 0;

					if (!pEntity->As<C_BaseEntity>()->IsInValidTeam(&nPlayerTeam))
						continue;

					m_mapGroups[EEntGroup::PLAYERS_ALL].push_back(pEntity);
					m_mapGroups[nLocalTeam != nPlayerTeam ? EEntGroup::PLAYERS_ENEMIES : EEntGroup::PLAYERS_TEAMMATES].push_back(pEntity);

					break;
				}

				case ETFClassIds::CObjectSentrygun:
				case ETFClassIds::CObjectDispenser:
				case ETFClassIds::CObjectTeleporter:
				{
					int nObjectTeam = 0;

					if (!pEntity->IsInValidTeam(&nObjectTeam))
						continue;

					m_mapGroups[EEntGroup::BUILDINGS_ALL].push_back(pEntity);
					m_mapGroups[nLocalTeam != nObjectTeam ? EEntGroup::BUILDINGS_ENEMIES : EEntGroup::BUILDINGS_TEAMMATES].push_back(pEntity);

					break;
				}

				case ETFClassIds::CTFProjectile_Rocket:
				case ETFClassIds::CTFProjectile_SentryRocket:
				case ETFClassIds::CTFProjectile_Jar:
				case ETFClassIds::CTFProjectile_JarGas:
				case ETFClassIds::CTFProjectile_JarMilk:
				case ETFClassIds::CTFProjectile_Arrow:
				case ETFClassIds::CTFProjectile_Flare:
				case ETFClassIds::CTFProjectile_Cleaver:
				case ETFClassIds::CTFProjectile_HealingBolt:
				case ETFClassIds::CTFGrenadePipebombProjectile:
				case ETFClassIds::CTFProjectile_BallOfFire:
				case ETFClassIds::CTFProjectile_EnergyRing:
				case ETFClassIds::CTFProjectile_EnergyBall:
				{
					int nProjectileTeam = 0;

					if (!pEntity->IsInValidTeam(&nProjectileTeam))
						continue;

					if (pEntity->GetClassId() == ETFClassIds::CTFGrenadePipebombProjectile)
					{
						auto pPipebomb = pEntity->As<C_TFGrenadePipebombProjectile>();

						/*if (pPipebomb->m_iType() == TF_GL_MODE_REMOTE_DETONATE_PRACTICE)
							continue;*/

						if (pPipebomb->HasStickyEffects() && pPipebomb->As<C_BaseGrenade>()->m_hThrower().Get() == pLocal)
							m_mapGroups[EEntGroup::PROJECTILES_LOCAL_STICKIES].push_back(pEntity);
					}

					m_mapGroups[EEntGroup::PROJECTILES_ALL].push_back(pEntity);
					m_mapGroups[nLocalTeam != nProjectileTeam ? EEntGroup::PROJECTILES_ENEMIES : EEntGroup::PROJECTILES_TEAMMATES].push_back(pEntity);

					break;
				}

				case ETFClassIds::CBaseAnimating:
				{
					if (IsHealthPack(pEntity))
						m_mapGroups[EEntGroup::HEALTHPACKS].push_back(pEntity);

					if (IsAmmoPack(pEntity))
						m_mapGroups[EEntGroup::AMMOPACKS].push_back(pEntity);

					break;
				}

				case ETFClassIds::CTFAmmoPack:
				{
					m_mapGroups[EEntGroup::AMMOPACKS].push_back(pEntity);
					break;
				}

				case ETFClassIds::CHalloweenGiftPickup:
				{
					m_mapGroups[EEntGroup::HALLOWEEN_GIFT].push_back(pEntity);

					break;
				}

				case ETFClassIds::CCurrencyPack:
				{
					if (pEntity->As<C_CurrencyPack>()->m_bDistributed())
					{
						continue;
					}

					m_mapGroups[EEntGroup::MVM_MONEY].push_back(pEntity);

					break;
				}

				default: break;
			}
		}
	}
}

void CEntityHelper::UpdateModelIndexes()
{
	m_mapHealthPacks.clear();
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_small.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_medium.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_large.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/props_halloween/halloween_medkit_small.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/props_halloween/halloween_medkit_medium.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/props_halloween/halloween_medkit_large.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_small_bday.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_medium_bday.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_large_bday.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/props_medieval/medieval_meat.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/plate.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/plate_sandwich_xmas.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/plate_robo_sandwich.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/workshop/weapons/c_models/c_fishcake/plate_fishcake.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/workshop/weapons/c_models/c_buffalo_steak/plate_buffalo_steak.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/workshop/weapons/c_models/c_chocolate/plate_chocolate.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/banana/plate_banana.mdl")] = true;

	m_mapAmmoPacks.clear();
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_small.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_medium.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_large.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_small_bday.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_medium_bday.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_large_bday.mdl")] = true;
}