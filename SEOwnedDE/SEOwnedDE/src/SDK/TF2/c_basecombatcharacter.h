#pragma once
#include "c_baseflex.h"

class C_BaseCombatCharacter : public C_BaseFlex
{
public:
	NETVAR(m_flNextAttack, float, "CBaseCombatCharacter", "m_flNextAttack");
	NETVAR(m_hActiveWeapon, EHANDLE, "CBaseCombatCharacter", "m_hActiveWeapon");
	NETVAR(m_hMyWeapons, EHANDLE, "CBaseCombatCharacter", "m_hMyWeapons");
	NETVAR(m_bGlowEnabled, bool, "CBaseCombatCharacter", "m_bGlowEnabled");
};