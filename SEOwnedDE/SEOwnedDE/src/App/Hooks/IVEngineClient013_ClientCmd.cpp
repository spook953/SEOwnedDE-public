#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

MAKE_HOOK(
	IVEngineClient013_ClientCmd, Memory::GetVFunc(I::EngineClient, 7),
	void, __fastcall, void *ecx, void *edx, const char *szCmdString)
{
	auto RunFakeTaunt = [&]()
	{
		if (!CFG::Misc_Fake_Taunt)
			return false;

		static const auto Hash0 = HASH_CT("taunt 0");
		static const auto Hash1 = HASH_CT("taunt 1");
		static const auto Hash2 = HASH_CT("taunt 2");
		static const auto Hash3 = HASH_CT("taunt 3");
		static const auto Hash4 = HASH_CT("taunt 4");
		static const auto Hash5 = HASH_CT("taunt 5");
		static const auto Hash6 = HASH_CT("taunt 6");
		static const auto Hash7 = HASH_CT("taunt 7");
		static const auto Hash8 = HASH_CT("taunt 8");
		static const auto Hash9 = HASH_CT("taunt");

		if ((HASH_RT(szCmdString) == Hash1
			|| HASH_RT(szCmdString) == Hash2
			|| HASH_RT(szCmdString) == Hash3
			|| HASH_RT(szCmdString) == Hash4
			|| HASH_RT(szCmdString) == Hash5
			|| HASH_RT(szCmdString) == Hash6
			|| HASH_RT(szCmdString) == Hash7
			|| HASH_RT(szCmdString) == Hash8
			|| HASH_RT(szCmdString) == Hash9)
			&& G::bStartedFakeTaunt)
			return true;

		if (HASH_RT(szCmdString) != Hash0 && HASH_RT(szCmdString) != Hash9)
			return false;

		if (G::bStartedFakeTaunt)
			return true;

		auto pLocal = H::Entities->GetLocal();

		if (!pLocal)
			return false;

		auto pWeapon = H::Entities->GetWeapon();

		if (!pWeapon)
			return false;

		auto pAnimState = pLocal->GetAnimState();

		if (!pAnimState)
			return false;

		auto &GestureSlotCustom = pAnimState->m_aGestureSlots[GESTURE_SLOT_VCD];

		auto pAnimLayer = GestureSlotCustom.m_pAnimLayer;

		if (!pAnimLayer)
			return false;

		auto GetSequenceName = [&](std::string &strOut) -> bool
		{
			int nWeaponId = pWeapon->GetWeaponID();
			int nClass = pLocal->m_iClass();
			int nItemIdx = pWeapon->m_iItemDefinitionIndex();

			if (nWeaponId == TF_WEAPON_LUNCHBOX && nClass == TF_CLASS_SCOUT)
				return false;

			if (nWeaponId == TF_WEAPON_FIREAXE && nClass == TF_CLASS_HEAVYWEAPONS)
				return false;

			if (nWeaponId == TF_WEAPON_JAR && nClass == TF_CLASS_SNIPER)
				return false;

			if (nWeaponId == TF_WEAPON_BUILDER && nClass == TF_CLASS_SPY)
				return false;

			strOut = std::string("taunt0" + std::to_string(pWeapon->GetSlot() + 1));

			if (nWeaponId == TF_WEAPON_LUNCHBOX && nClass == TF_CLASS_HEAVYWEAPONS)
				strOut = "taunt04";

			if (nWeaponId == TF_WEAPON_PARTICLE_CANNON && nClass == TF_CLASS_SOLDIER)
				strOut = "taunt08";

			if (nWeaponId == TF_WEAPON_SHOTGUN_SOLDIER && nClass == TF_CLASS_SOLDIER)
				strOut = "taunt04";

			if (nWeaponId == TF_WEAPON_BUFF_ITEM && nClass == TF_CLASS_SOLDIER)
				strOut = "taunt02";

			if (nWeaponId == TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT && nClass == TF_CLASS_SOLDIER)
				strOut = "taunt07";

			if (nWeaponId == TF_WEAPON_ROCKETPACK && nClass == TF_CLASS_PYRO)
				strOut = "taunt_gasblast";

			if (nWeaponId == TF_WEAPON_SLAP && nClass == TF_CLASS_PYRO)
				strOut = "taunt02";

			if ((nWeaponId == TF_WEAPON_GRENADELAUNCHER || nWeaponId == TF_WEAPON_CANNON) && nClass == TF_CLASS_DEMOMAN)
				strOut = "taunt02";

			if (nWeaponId == TF_WEAPON_STICKBOMB && nClass == TF_CLASS_DEMOMAN)
				strOut = "taunt04";

			if (nWeaponId == TF_WEAPON_SENTRY_REVENGE && nClass == TF_CLASS_ENGINEER)
				strOut = "taunt07";

			if (nWeaponId == TF_WEAPON_COMPOUND_BOW && nClass == TF_CLASS_SNIPER)
				strOut = "taunt04";

			if (nWeaponId == TF_WEAPON_BAT_WOOD && nClass == TF_CLASS_SCOUT)
				strOut = "taunt05";

			if (nWeaponId == TF_WEAPON_SHOVEL && nClass == TF_CLASS_SOLDIER)
			{
				if (nItemIdx == Soldier_t_TheEqualizer || nItemIdx == Soldier_t_TheEscapePlan)
					strOut = "taunt05";
			}

			if (nWeaponId == TF_WEAPON_BONESAW && nClass == TF_CLASS_MEDIC)
			{
				if (nItemIdx == Medic_t_TheUbersaw)
					strOut = "taunt08";
			}

			if (nWeaponId == TF_WEAPON_MEDIGUN && nClass == TF_CLASS_MEDIC)
			{
				if (nItemIdx == Medic_s_TheKritzkrieg)
					strOut = "taunt06";
			}

			if (nWeaponId == TF_WEAPON_WRENCH && nClass == TF_CLASS_ENGINEER)
			{
				if (nItemIdx == Engi_t_TheGunslinger)
					strOut = "taunt09";
			}

			if (nWeaponId == TF_WEAPON_SWORD && nClass == TF_CLASS_DEMOMAN)
			{
				if (nItemIdx == Demoman_t_TheScotsmansSkullcutter)
					strOut = "taunt02";

				else strOut = "taunt09";
			}

			if (nWeaponId == TF_WEAPON_BOTTLE && nClass == TF_CLASS_DEMOMAN)
			{
				if (nItemIdx == Demoman_t_ThePainTrain)
					strOut = "taunt02";
			}

			if (nWeaponId == TF_WEAPON_PIPEBOMBLAUNCHER && nClass == TF_CLASS_DEMOMAN)
			{
				if (nItemIdx == Demoman_s_TheScottishResistance)
					strOut = "taunt08";

				else strOut = "taunt01";
			}

			if (nWeaponId == TF_WEAPON_FISTS && nClass == TF_CLASS_HEAVYWEAPONS)
			{
				if (nItemIdx == Heavy_t_FestiveGlovesofRunningUrgently
					|| nItemIdx == Heavy_t_FistsofSteel
					|| nItemIdx == Heavy_t_GlovesofRunningUrgently
					|| nItemIdx == Heavy_t_TheKillingGlovesofBoxing
					|| nItemIdx == Heavy_t_TheBreadBite
					|| nItemIdx == Heavy_t_TheEvictionNotice
					|| nItemIdx == Heavy_t_WarriorsSpirit)
					strOut = "taunt05";
			}

			if (nWeaponId == TF_WEAPON_FLAREGUN && nClass == TF_CLASS_PYRO)
			{
				if (nItemIdx == Pyro_s_TheScorchShot)
					strOut = "taunt_scorch_shot";
			}

			return true;
		};

		std::string sSequence = {};

		if (!GetSequenceName(sSequence))
			return false;

		GestureSlotCustom.m_bActive = true;
		GestureSlotCustom.m_bAutoKill = true;
		GestureSlotCustom.m_iGestureSlot = GESTURE_SLOT_VCD;
		GestureSlotCustom.m_iActivity = ACT_MP_VCD;
		
		pAnimLayer->m_nSequence = SDKUtils::LookupSequence(pLocal->GetModelPtr(), sSequence.c_str());
		pAnimLayer->m_flPrevCycle = 0.0f;
		pAnimLayer->m_flWeight = 1.0f;
		pAnimLayer->m_nOrder = 5;
		pAnimLayer->m_flPlaybackRate = 1.0f;
		pAnimLayer->m_flCycle = 0.0f;
		pAnimLayer->m_flLayerAnimtime = 0.0f;
		pAnimLayer->m_flLayerFadeOuttime = 0.0f;
		pAnimLayer->m_flBlendIn = 0.0f;
		pAnimLayer->m_flBlendOut = 0.0f;
		pAnimLayer->m_bClientBlend = false;

		G::bStartedFakeTaunt = true;
		G::flFakeTauntStartYaw = G::vUserCmdAngles.y;

		pLocal->m_nForceTauntCam() = 1;

		return true;
	};

	if (RunFakeTaunt())
		return;

	CALL_ORIGINAL(ecx, edx, szCmdString);
}