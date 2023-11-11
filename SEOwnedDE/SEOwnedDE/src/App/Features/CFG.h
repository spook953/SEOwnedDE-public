#pragma once

#include "../../Utils/Config/Config.h"

namespace CFG
{
#pragma region Aimbot

	CFGVAR(Aimbot_Active, true);
	CFGVAR(Aimbot_AutoShoot, true);
	CFGVAR(Aimbot_Key, 0);
	CFGVAR(Aimbot_Target_Players, true);
	CFGVAR(Aimbot_Target_Buildings, true);
	CFGVAR(Aimbot_Ignore_Friends, true);
	CFGVAR(Aimbot_Ignore_Invisible, false);
	CFGVAR(Aimbot_Ignore_Invulnerable, true);
	CFGVAR(Aimbot_Ignore_Taunting, false);

	CFGVAR(Aimbot_Hitscan_Active, true);
	CFGVAR(Aimbot_Hitscan_Target_LagRecords, true);
	CFGVAR(Aimbot_Hitscan_Target_Stickies, true);
	CFGVAR(Aimbot_Hitscan_Aim_Type, 1); //0 Normal 1 Silent 2 Smooth
	CFGVAR(Aimbot_Hitscan_Sort, 0); //0 FOV 1 Distance
	CFGVAR(Aimbot_Hitscan_Hitbox, 2); //0 Head 1 Body 2 Auto
	CFGVAR(Aimbot_Hitscan_FOV, 45.0f);
	CFGVAR(Aimbot_Hitscan_Smoothing, 5.0f);
	CFGVAR(Aimbot_Hitscan_Scan_Head, true);
	CFGVAR(Aimbot_Hitscan_Scan_Body, true);
	CFGVAR(Aimbot_Hitscan_Scan_Arms, false);
	CFGVAR(Aimbot_Hitscan_Scan_Legs, false);
	CFGVAR(Aimbot_Hitscan_Scan_Buildings, true);
	CFGVAR(Aimbot_Hitscan_Advanced_Smooth_AutoShoot, true);
	CFGVAR(Aimbot_Hitscan_Auto_Scope, false);
	CFGVAR(Aimbot_Hitscan_Wait_For_Headshot, true);
	CFGVAR(Aimbot_Hitscan_Wait_For_Charge, false);
	CFGVAR(Aimbot_Hitscan_Minigun_TapFire, false);

	CFGVAR(Aimbot_Projectile_Active, true);
	CFGVAR(Aimbot_Projectile_NoSpread, true);
	CFGVAR(Aimbot_Projectile_Auto_Double_Donk, true);
	CFGVAR(Aimbot_Projectile_Advanced_Head_Aim, true);
	CFGVAR(Aimbot_Projectile_Ground_Strafe_Prediction, true);
	CFGVAR(Aimbot_Projectile_Air_Strafe_Prediction, true);
	CFGVAR(Aimbot_Projectile_BBOX_Multipoint, true);
	CFGVAR(Aimbot_Projectile_Rocket_Splash, 2); //0 Disabled 1 Enabled 2 Preferred
	CFGVAR(Aimbot_Projectile_Aim_Type, 1); //0 Normal 1 Silent
	CFGVAR(Aimbot_Projectile_Sort, 0); //0 FOV 1 Distance
	CFGVAR(Aimbot_Projectile_Aim_Position, 3); //0 Feet 1 Body 2 Head 3 Auto
	CFGVAR(Aimbot_Projectile_Aim_Prediction_Method, 0); //0 Full Acceleration 1 Current Velocity
	CFGVAR(Aimbot_Projectile_FOV, 45.0f);
	CFGVAR(Aimbot_Projectile_Max_Simulation_Time, 1.5f);
	CFGVAR(Aimbot_Projectile_Max_Processing_Targets, 1);

	CFGVAR(Aimbot_Melee_Active, true);
	CFGVAR(Aimbot_Melee_Always_Active, false);
	CFGVAR(Aimbot_Melee_Target_LagRecords, true);
	CFGVAR(Aimbot_Melee_Aim_Type, 1); //0 Normal 1 Silent 2 Smooth
	CFGVAR(Aimbot_Melee_Sort, 1); //0 FOV 1 Distance
	CFGVAR(Aimbot_Melee_FOV, 45.0f);
	CFGVAR(Aimbot_Melee_Smoothing, 5.0f);
	CFGVAR(Aimbot_Melee_Predict_Swing, true);
	CFGVAR(Aimbot_Melee_Predict_Swing_Amount, 0.18f);
	CFGVAR(Aimbot_Melee_Walk_To_Target, false);
	CFGVAR(Aimbot_Melee_Whip_Teammates, true);

#pragma endregion

#pragma region Triggerbot

	CFGVAR(Triggerbot_Active, true);
	CFGVAR(Triggerbot_Key, 0);

	CFGVAR(Triggerbot_AutoBackstab_Active, true);
	CFGVAR(Triggerbot_AutoBackstab_Knife_If_Lethal, true);
	CFGVAR(Triggerbot_AutoBacktab_Mode, 1); //0 Legit 1 Rage
	CFGVAR(Triggerbot_AutoBacktab_Aim_Mode, 1); //0 Normal 1 Silent
	CFGVAR(Triggerbot_AutoBackstab_Ignore_Friends, true);
	CFGVAR(Triggerbot_AutoBackstab_Ignore_Invisible, false);
	CFGVAR(Triggerbot_AutoBackstab_Ignore_Invulnerable, true);

	CFGVAR(Triggerbot_AutoDetonate_Active, true);
	CFGVAR(Triggerbot_AutoDetonate_Target_Players, true);
	CFGVAR(Triggerbot_AutoDetonate_Target_Buildings, true);
	CFGVAR(Triggerbot_AutoDetonate_Ignore_Friends, true);
	CFGVAR(Triggerbot_AutoDetonate_Ignore_Invisible, false);
	CFGVAR(Triggerbot_AutoDetonate_Ignore_Invulnerable, false);

	CFGVAR(Triggerbot_AutoAirblast_Active, true);
	CFGVAR(Triggerbot_AutoAirblast_Aim_Assist, true);
	CFGVAR(Triggerbot_AutoAirblast_Mode, 1); //0 Legit 1 Rage
	CFGVAR(Triggerbot_AutoAirblast_Aim_Mode, 1); //0 Normal 1 Silent
	CFGVAR(Triggerbot_AutoAirblast_Ignore_Rocket, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_SentryRocket, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_Jar, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_JarGas, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_JarMilk, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_Arrow, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_Flare, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_Cleaver, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_HealingBolt, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_PipebombProjectile, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_BallOfFire, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_EnergyRing, false);
	CFGVAR(Triggerbot_AutoAirblast_Ignore_EnergyBall, false);

#pragma endregion

#pragma region AutoVaccinator

	CFGVAR(AutoVaccinator_Active, true);
	CFGVAR(AutoVaccinator_Pop, 0); //0 everyone 1 friends only

#pragma endregion

#pragma region ESP

	CFGVAR(ESP_Active, true);
	CFGVAR(ESP_Tracer_From, 1); //0 Top 1 Center 2 Bottom
	CFGVAR(ESP_Tracer_To, 2); //0 Top 1 Center 2 Bottom
	CFGVAR(ESP_Text_Color, 1); //0 Default 1 White

	CFGVAR(ESP_Players_Active, true);
	CFGVAR(ESP_Players_Alpha, 0.7f);
	CFGVAR(ESP_Players_Bones_Color, 1); //0 Default 1 White
	CFGVAR(ESP_Players_Arrows_Radius, 100.0f);
	CFGVAR(ESP_Players_Arrows_Max_Distance, 500.0f);
	CFGVAR(ESP_Players_Ignore_Local, false);
	CFGVAR(ESP_Players_Ignore_Friends, false);
	CFGVAR(ESP_Players_Ignore_Enemies, false);
	CFGVAR(ESP_Players_Ignore_Teammates, true);
	CFGVAR(ESP_Players_Ignore_Invisible, false);
	CFGVAR(ESP_Players_Show_Teammate_Medics, true);
	CFGVAR(ESP_Players_Name, true);
	CFGVAR(ESP_Players_Class, false);
	CFGVAR(ESP_Players_Class_Icon, false);
	CFGVAR(ESP_Players_Health, false);
	CFGVAR(ESP_Players_HealthBar, true);
	CFGVAR(ESP_Players_Uber, false);
	CFGVAR(ESP_Players_UberBar, true);
	CFGVAR(ESP_Players_Box, false);
	CFGVAR(ESP_Players_Tracer, false);
	CFGVAR(ESP_Players_Bones, false);
	CFGVAR(ESP_Players_Arrows, true);
	CFGVAR(ESP_Players_Conds, true);
	CFGVAR(ESP_Players_Sniper_Lines, true);

	CFGVAR(ESP_Buildings_Active, true);
	CFGVAR(ESP_Buildings_Alpha, 0.7f);
	CFGVAR(ESP_Buildings_Ignore_Local, false);
	CFGVAR(ESP_Buildings_Ignore_Enemies, false);
	CFGVAR(ESP_Buildings_Ignore_Teammates, true);
	CFGVAR(ESP_Buildings_Show_Teammate_Dispensers, true);
	CFGVAR(ESP_Buildings_Name, true);
	CFGVAR(ESP_Buildings_Health, false);
	CFGVAR(ESP_Buildings_HealthBar, true);
	CFGVAR(ESP_Buildings_Level, false);
	CFGVAR(ESP_Buildings_LevelBar, false);
	CFGVAR(ESP_Buildings_Box, false);
	CFGVAR(ESP_Buildings_Tracer, false);
	CFGVAR(ESP_Buildings_Conds, true);

	CFGVAR(ESP_World_Active, false);
	CFGVAR(ESP_World_Alpha, 1.0f);
	CFGVAR(ESP_World_Ignore_HealthPacks, false);
	CFGVAR(ESP_World_Ignore_AmmoPacks, false);
	CFGVAR(ESP_World_Ignore_LocalProjectiles, false);
	CFGVAR(ESP_World_Ignore_EnemyProjectiles, false);
	CFGVAR(ESP_World_Ignore_TeammateProjectiles, true);
	CFGVAR(ESP_World_Ignore_Halloween_Gift, false);
	CFGVAR(ESP_World_Ignore_MVM_Money, false);
	CFGVAR(ESP_World_Name, true);
	CFGVAR(ESP_World_Box, false);
	CFGVAR(ESP_World_Tracer, false);

#pragma endregion

#pragma region Radar

	CFGVAR(Radar_Active, true);
	CFGVAR(Radar_Style, 1); //0 Rectangle 1 Circle
	CFGVAR(Radar_Size, 250);
	CFGVAR(Radar_Icon_Size, 18);
	CFGVAR(Radar_Radius, 1200.0f);
	CFGVAR(Radar_Cross_Alpha, 0.5f);
	CFGVAR(Radar_Outline_Alpha, 1.0f);
	CFGVAR(Radar_Background_Alpha, 0.9f);
	CFGVAR(Radar_Pos_X, 20);
	CFGVAR(Radar_Pos_Y, 20);

	CFGVAR(Radar_Players_Active, true);
	CFGVAR(Radar_Players_Ignore_Local, true);
	CFGVAR(Radar_Players_Ignore_Friends, false);
	CFGVAR(Radar_Players_Ignore_Teammates, true);
	CFGVAR(Radar_Players_Ignore_Enemies, false);
	CFGVAR(Radar_Players_Ignore_Invisible, false);
	CFGVAR(Radar_Players_Show_Teammate_Medics, true);

	CFGVAR(Radar_Buildings_Active, true);
	CFGVAR(Radar_Buildings_Ignore_Local, false);
	CFGVAR(Radar_Buildings_Ignore_Teammates, true);
	CFGVAR(Radar_Buildings_Ignore_Enemies, false);
	CFGVAR(Radar_Buildings_Show_Teammate_Dispensers, true);

	CFGVAR(Radar_World_Active, true);
	CFGVAR(Radar_World_Ignore_HealthPacks, false);
	CFGVAR(Radar_World_Ignore_AmmoPacks, false);
	CFGVAR(Radar_World_Ignore_Halloween_Gift, false);
	CFGVAR(Radar_World_Ignore_MVM_Money, false);

#pragma endregion

#pragma region Materials

	CFGVAR(Materials_Active, true);

	CFGVAR(Materials_Players_Active, true);
	CFGVAR(Materials_Players_No_Depth, false);
	CFGVAR(Materials_Players_Alpha, 1.0f);
	CFGVAR(Materials_Players_Material, 0); //0 Original 1 Flat 2 Shaded 3 Glossy 4 Glow 5 Plastic
	CFGVAR(Materials_Players_Ignore_Local, false);
	CFGVAR(Materials_Players_Ignore_Friends, false);
	CFGVAR(Materials_Players_Ignore_Enemies, false);
	CFGVAR(Materials_Players_Ignore_Teammates, true);
	CFGVAR(Materials_Players_Ignore_LagRecords, false);
	CFGVAR(Materials_Players_Show_Teammate_Medics, false);
	CFGVAR(Materials_Players_LagRecords_Style, 1); //0 All 1 Last Only

	CFGVAR(Materials_Buildings_Active, false);
	CFGVAR(Materials_Buildings_No_Depth, false);
	CFGVAR(Materials_Buildings_Alpha, 1.0f);
	CFGVAR(Materials_Buildings_Material, 2); //0 Original 1 Flat 2 Shaded 3 Glossy 4 Glow 5 Plastic
	CFGVAR(Materials_Buildings_Ignore_Local, false);
	CFGVAR(Materials_Buildings_Ignore_Enemies, false);
	CFGVAR(Materials_Buildings_Ignore_Teammates, false);
	CFGVAR(Materials_Buildings_Show_Teammate_Dispensers, false);

	CFGVAR(Materials_World_Active, false);
	CFGVAR(Materials_World_No_Depth, false);
	CFGVAR(Materials_World_Alpha, 1.0f);
	CFGVAR(Materials_World_Material, 2); //0 Original 1 Flat 2 Shaded 3 Glossy 4 Glow 5 Plastic
	CFGVAR(Materials_World_Ignore_HealthPacks, false);
	CFGVAR(Materials_World_Ignore_AmmoPacks, false);
	CFGVAR(Materials_World_Ignore_LocalProjectiles, false);
	CFGVAR(Materials_World_Ignore_EnemyProjectiles, false);
	CFGVAR(Materials_World_Ignore_TeammateProjectiles, false);
	CFGVAR(Materials_World_Ignore_Halloween_Gift, false);
	CFGVAR(Materials_World_Ignore_MVM_Money, false);

	CFGVAR(Materials_ViewModel_Active, false);
	CFGVAR(Materials_ViewModel_Hands_Alpha, 1.0f);
	CFGVAR(Materials_ViewModel_Hands_Material, 0); //0 Original 1 Flat 2 Shaded 3 Glossy 4 Glow 5 Plastic
	CFGVAR(Materials_ViewModel_Weapon_Alpha, 1.0f);
	CFGVAR(Materials_ViewModel_Weapon_Material, 0); //0 Original 1 Flat 2 Shaded 3 Glossy 4 Glow 5 Plastic

#pragma endregion

#pragma region Outlines

	CFGVAR(Outlines_Active, true);
	CFGVAR(Outlines_Style, 1); //0 Bloom 1 Crisp 2 Cartoony 3 Cartoony Alt
	CFGVAR(Outlines_Bloom_Amount, 5);

	CFGVAR(Outlines_Players_Active, true);
	CFGVAR(Outlines_Players_Alpha, 1.0f);
	CFGVAR(Outlines_Players_Ignore_Local, false);
	CFGVAR(Outlines_Players_Ignore_Friends, false);
	CFGVAR(Outlines_Players_Ignore_Enemies, false);
	CFGVAR(Outlines_Players_Ignore_Teammates, true);
	CFGVAR(Outlines_Players_Show_Teammate_Medics, true);

	CFGVAR(Outlines_Buildings_Active, true);
	CFGVAR(Outlines_Buildings_Alpha, 1.0f);
	CFGVAR(Outlines_Buildings_Ignore_Local, false);
	CFGVAR(Outlines_Buildings_Ignore_Enemies, false);
	CFGVAR(Outlines_Buildings_Ignore_Teammates, true);
	CFGVAR(Outlines_Buildings_Show_Teammate_Dispensers, true);

	CFGVAR(Outlines_World_Active, true);
	CFGVAR(Outlines_World_Alpha, 1.0f);
	CFGVAR(Outlines_World_Ignore_HealthPacks, false);
	CFGVAR(Outlines_World_Ignore_AmmoPacks, false);
	CFGVAR(Outlines_World_Ignore_LocalProjectiles, false);
	CFGVAR(Outlines_World_Ignore_EnemyProjectiles, false);
	CFGVAR(Outlines_World_Ignore_TeammateProjectiles, true);
	CFGVAR(Outlines_World_Ignore_Halloween_Gift, false);
	CFGVAR(Outlines_World_Ignore_MVM_Money, false);

#pragma endregion

#pragma region OtherVisuals

	CFGVAR(Visuals_Aimbot_FOV_Circle, true);
	CFGVAR(Visuals_Aimbot_FOV_Circle_Alpha, 0.03f);
	CFGVAR(Visuals_Draw_Projectile_Arc, false);
	CFGVAR(Visuals_Draw_Movement_Path_Style, 2); //0 Disabled 1 Line 2 Dashed Line 3 Alt Line
	CFGVAR(Visuals_Draw_Projectile_Arc_Color_Mode, 0); //0 Custom 1 Rainbow
	CFGVAR(Visuals_FOV_Override, 120.0f);
	CFGVAR(Visuals_Remove_Scope, true);
	CFGVAR(Visuals_Remove_Zoom, true);
	CFGVAR(Visuals_Remove_Punch, true);
	CFGVAR(Visuals_Remove_Screen_Overlay, true);
	CFGVAR(Visuals_Remove_Screen_Shake, true);
	CFGVAR(Visuals_Remove_Screen_Fade, true);
	CFGVAR(Visuals_Removals_Mode, 0); //0 Everyone 1 Local Only
	CFGVAR(Visuals_Reveal_Scoreboard, true);
	CFGVAR(Visuals_Tracer_Type, 0); //0 Default 1 C.A.P.P.E.R 2 Machina (White) 3 Machina (Team) 4 Big Nasty 5 Short Circuit 6 Mrasmus Zap 7 Random 8 Random (No Merasmus Zap)

	CFGVAR(Visuals_ViewModel_Active, true);
	CFGVAR(Visuals_ViewModel_Sway, true);
	CFGVAR(Visuals_ViewModel_Sway_Scale, 0.1f);
	CFGVAR(Visuals_ViewModel_Offset_Forward, 0.0f);
	CFGVAR(Visuals_ViewModel_Offset_Right, 0.0f);
	CFGVAR(Visuals_ViewModel_Offset_Up, 0.0f);

	CFGVAR(Visuals_Flat_Textures, false);
	CFGVAR(Visuals_Remove_Fog, true);
	CFGVAR(Visuals_Remove_Sky_Fog, true);
	CFGVAR(Visuals_Night_Mode, 50.0f);
	CFGVAR(Visuals_World_Modulation_Mode, 0); //0 Night 1 Custom Colors
	CFGVAR(Visuals_World_Modulation_No_Sky_Change, false);
	CFGVAR(Visuals_Distance_Prop_Alpha, false);

	CFGVAR(Visuals_Thirdperson_Active, false);
	CFGVAR(Visuals_Thirdperson_Key, 0);
	CFGVAR(Visuals_Thirdperson_Offset_Forward, 150.0f);
	CFGVAR(Visuals_Thirdperson_Offset_Right, 0.0f);
	CFGVAR(Visuals_Thirdperson_Offset_Up, 0.0f);

	CFGVAR(Visuals_SpectatorList_Active, true);
	CFGVAR(Visuals_SpectatorList_Outline_Alpha, 1.0f);
	CFGVAR(Visuals_SpectatorList_Background_Alpha, 0.9f);
	CFGVAR(Visuals_SpectatorList_Pos_X, 400);
	CFGVAR(Visuals_SpectatorList_Pos_Y, 20);
	CFGVAR(Visuals_SpectatorList_Width, 200);

	CFGVAR(Visuals_Ragdolls_Active, false);
	CFGVAR(Visuals_Ragdolls_No_Gib, false);
	CFGVAR(Visuals_Ragdolls_No_Death_Anim, false);
	CFGVAR(Visuals_Ragdolls_Effect, 0); //0 Default 1 Burning 2 Electrocuted 3 Ash 4 Gold 5 Ice 6 Dissolve 7 Random
	CFGVAR(Visuals_Ragdolls_Force_Mult, 1.0f);

	CFGVAR(Visuals_Paint_Active, false);
	CFGVAR(Visuals_Paint_Key, 0);
	CFGVAR(Visuals_Paint_Erase_Key, 0);
	CFGVAR(Visuals_Paint_LifeTime, 2.0f);
	CFGVAR(Visuals_Paint_Bloom_Amount, 7);

	CFGVAR(Visuals_Disable_Detail_Props, false);
	CFGVAR(Visuals_Disable_Ragdolls, false);
	CFGVAR(Visuals_Disable_Wearables, false);
	CFGVAR(Visuals_Disable_Post_Processing, true);
	CFGVAR(Visuals_Disable_Dropped_Weapons, false);
	CFGVAR(Visuals_Simple_Models, false);
	CFGVAR(Visuals_Particles_Mode, 0); //0 Original 1 Custom Color 2 Rainbow
	CFGVAR(Visuals_Particles_Rainbow_Rate, 3.0f);

	CFGVAR(Visuals_Beams_Active, false);
	CFGVAR(Visuals_Beams_LifeTime, 2.0f);
	CFGVAR(Visuals_Beams_Width, 2.0f);
	CFGVAR(Visuals_Beams_EndWidth, 2.0f);
	CFGVAR(Visuals_Beams_FadeLength, 10.0f);
	CFGVAR(Visuals_Beams_Amplitude, 0.2f);
	CFGVAR(Visuals_Beams_Speed, 0.0f);
	CFGVAR(Visuals_Beams_Flag_FBEAM_FADEIN, false);
	CFGVAR(Visuals_Beams_Flag_FBEAM_FADEOUT, false);
	CFGVAR(Visuals_Beams_Flag_FBEAM_SINENOISE, false);
	CFGVAR(Visuals_Beams_Flag_FBEAM_SOLID, false);
	CFGVAR(Visuals_Beams_Flag_FBEAM_SHADEIN, false);
	CFGVAR(Visuals_Beams_Flag_FBEAM_SHADEOUT, false);

	CFGVAR(Visuals_SpyCamera_Active, false);
	CFGVAR(Visuals_SpyCamera_Background_Alpha, 0.9f);
	CFGVAR(Visuals_SpyCamera_Pos_X, 750);
	CFGVAR(Visuals_SpyCamera_Pos_Y, 20);
	CFGVAR(Visuals_SpyCamera_Pos_W, 400);
	CFGVAR(Visuals_SpyCamera_Pos_H, 250);
	CFGVAR(Visuals_SpyCamera_FOV, 90.0f);

	CFGVAR(Viuals_SpyWarning_Active, true);
	CFGVAR(Viuals_SpyWarning_Announce, false);
	CFGVAR(Viuals_SpyWarning_Ignore_Cloaked, false);
	CFGVAR(Viuals_SpyWarning_Ignore_Friends, false);
	CFGVAR(Viuals_SpyWarning_Ignore_Invisible, false);

	CFGVAR(Visuals_TeamWellBeing_Active, true);
	CFGVAR(Visuals_TeamWellBeing_Medic_Only, false);
	CFGVAR(Visuals_TeamWellBeing_Background_Alpha, 0.9f);
	CFGVAR(Visuals_TeamWellBeing_Pos_X, 700);
	CFGVAR(Visuals_TeamWellBeing_Pos_Y, 20);
	CFGVAR(Visuals_TeamWellBeing_Width, 200);

	CFGVAR(Visuals_Custom_Skybox_Texture_Name, std::string({ "" }));

	CFGVAR(Visuals_Chat_Teammate_Votes, true);
	CFGVAR(Visuals_Chat_Enemy_Votes, false);
	CFGVAR(Visuals_Chat_Player_List_Info, true);
	CFGVAR(Visuals_Chat_Name_Tags, true);

#pragma endregion

#pragma region Misc

	CFGVAR(Misc_Bunnyhop, true);
	CFGVAR(Misc_Choke_On_Bhop, false);
	CFGVAR(Misc_Taunt_Slide, false);
	CFGVAR(Misc_Taunt_Slide_Control, false);
	CFGVAR(Misc_Taunt_Spin_Key, 0);
	CFGVAR(Misc_Taunt_Spin_Speed, 12.0f);
	CFGVAR(Misc_Taunt_Spin_Sine, false);
	CFGVAR_NOSAVE(Misc_Accuracy_Improvements, true);
	CFGVAR(Misc_Pure_Bypass, true);
	CFGVAR(Misc_NoiseMaker_Spam, false);
	CFGVAR(Misc_No_Push, true);
	CFGVAR(Misc_MVM_Giant_Weapon_Sounds, false);
	CFGVAR(Misc_Fake_Taunt, false);
	CFGVAR(Misc_Ping_Reducer, true);
	CFGVAR(Misc_Pred_Error_Jitter_Fix, true);
	CFGVAR(Misc_SetupBones_Optimization, true);
	CFGVAR(Misc_ComputeLightingOrigin_Fix, true);
	CFGVAR(Misc_Equip_Region_Unlock, true);
	CFGVAR(Misc_Fast_Stop, false);
	CFGVAR(Misc_Auto_Strafe, false);
	CFGVAR(Misc_Auto_Strafe_Turn_Scale, 0.5f);
	CFGVAR(Misc_Prevent_Server_Angle_Change, false);
	CFGVAR(Misc_Edge_Jump_Key, 0);
	CFGVAR(Misc_Auto_Rocket_Jump_Key, 0);
	CFGVAR(Misc_Auto_Air_Pogo_Key, 0);
	CFGVAR(Misc_Auto_Disguise, false);
	CFGVAR(Misc_Auto_Medigun_Key, 0);
	CFGVAR(Misc_Movement_Lock_Key, 0);
	CFGVAR(Misc_Clean_Screenshot, true);

	CFGVAR(Misc_MVM_Instant_Respawn_Key, 0);
	CFGVAR(Misc_MVM_Instant_Revive, false);

	CFGVAR(Exploits_Shifting_Recharge_Key, 0);

	CFGVAR(Exploits_RapidFire_Key, 0);
	CFGVAR(Exploits_RapidFire_Ticks, 22);
	CFGVAR(Exploits_RapidFire_Min_Ticks_Target_Same, 3);
	CFGVAR(Exploits_RapidFire_Antiwarp, true);
	CFGVAR(Exploits_Warp_Key, 0);
	CFGVAR(Exploits_Warp_Mode, 0); //0 Slow 1 Full
	CFGVAR(Exploits_Warp_Exploit, 0); //0 None 1 Fake Peek 2 0 Velocity
	CFGVAR(Exploits_Shifting_Draw_Indicator, true);
	CFGVAR(Exploits_Shifting_Indicator_Style, 0); //0 Rectangle 1 Circle

	CFGVAR(Exploits_Crits_Force_Crit_Key, 0);
	CFGVAR(Exploits_Crits_Force_Crit_Key_Melee, 0);
	CFGVAR(Exploits_Crits_Skip_Random_Crits, true);

	CFGVAR(Exploits_SeedPred_Active, true);
	CFGVAR(Exploits_SeedPred_DrawIndicator, true);

	CFGVAR(Misc_Chat_Medieval, false);
	CFGVAR(Misc_Chat_Owoify, false);

#pragma endregion

#pragma region Colors

	CFGVAR(Color_Local, Color_t({ 43, 203, 186, 255 }));
	CFGVAR(Color_Friend, Color_t({ 38, 222, 129, 255 }));
	CFGVAR(Color_Enemy, Color_t({ 250, 170, 10, 255 }));
	CFGVAR(Color_Teammate, Color_t({ 252, 92, 101, 255 }));
	CFGVAR(Color_Target, Color_t({ 200, 200, 200, 255 }));
	CFGVAR(Color_Invulnerable, Color_t({ 165, 94, 234, 255 }));
	CFGVAR(Color_Invisible, Color_t({ 209, 216, 224, 255 }));
	CFGVAR(Color_Cheater, Color_t({ 214, 48, 49, 255 }));
	CFGVAR(Color_RetardLegit, Color_t({ 48, 218, 70, 255 }));
	CFGVAR(Color_OverHeal, Color_t({ 69, 170, 242, 255 }));
	CFGVAR(Color_Uber, Color_t({ 224, 86, 253, 255 }));
	CFGVAR(Color_Conds, Color_t({ 249, 202, 36, 255 }));
	CFGVAR(Color_HealthPack, Color_t({ 46, 204, 113, 255 }));
	CFGVAR(Color_AmmoPack, Color_t({ 200, 200, 200, 255 }));
	CFGVAR(Color_Beams, Color_t({ 200, 200, 200, 255 }));
	CFGVAR(Color_Halloween_Gift, Color_t({ 255, 255, 255, 255 }));
	CFGVAR(Color_MVM_Money, Color_t({ 0, 200, 20, 255 }));
	CFGVAR(Color_Particles, Color_t({ 0, 255, 0, 255 }));
	CFGVAR(Color_World, Color_t({ 255, 255, 255, 255 }));
	CFGVAR(Color_Sky, Color_t({ 255, 255, 255, 255 }));
	CFGVAR(Color_Props, Color_t({ 255, 255, 255, 255 }));
	CFGVAR(Color_Hands, Color_t({ 255, 255, 255, 255 }));
	CFGVAR(Color_Hands_Sheen, Color_t({ 255, 255, 255, 255 }));
	CFGVAR(Color_Weapon, Color_t({ 255, 255, 255, 255 }));
	CFGVAR(Color_Weapon_Sheen, Color_t({ 255, 255, 255, 255 }));
	CFGVAR(Color_Projectile_Arc, Color_t({ 255, 255, 255, 255 }));

	CFGVAR_NOSAVE(Color_ESP_Text, Color_t({ 200, 200, 200, 255 }));
	CFGVAR_NOSAVE(Color_ESP_Outline, Color_t({ 10, 10, 10, 255 }));

#pragma endregion

#pragma region Menu

	CFGVAR_NOSAVE(Menu_Pos_X, 500);
	CFGVAR_NOSAVE(Menu_Pos_Y, 200);
	CFGVAR_NOSAVE(Menu_Width, 555);
	CFGVAR_NOSAVE(Menu_Height, 420);
	CFGVAR_NOSAVE(Menu_Drag_Bar_Height, 15);

	CFGVAR_NOSAVE(Menu_Spacing_X, 3);
	CFGVAR_NOSAVE(Menu_Spacing_Y, 3);

	CFGVAR_NOSAVE(Menu_Tab_Button_Width, 60);
	CFGVAR_NOSAVE(Menu_Tab_Button_Height, 18);

	CFGVAR_NOSAVE(Menu_CheckBox_Width, 10);
	CFGVAR_NOSAVE(Menu_CheckBox_Height, 10);

	CFGVAR_NOSAVE(Menu_Slider_Width, 100);
	CFGVAR_NOSAVE(Menu_Slider_Height, 6);

	CFGVAR_NOSAVE(Menu_InputKey_Width, 60);
	CFGVAR_NOSAVE(Menu_InputKey_Height, 14);

	CFGVAR_NOSAVE(Menu_InputText_Width, 150);
	CFGVAR_NOSAVE(Menu_InputText_Height, 30);

	CFGVAR_NOSAVE(Menu_Select_Width, 120);
	CFGVAR_NOSAVE(Menu_Select_Height, 14);

	CFGVAR_NOSAVE(Menu_ColorPicker_Preview_Width, 10);
	CFGVAR_NOSAVE(Menu_ColorPicker_Preview_Height, 10);

	CFGVAR(Menu_Text, Color_t({ 240, 240, 240, 255 }));
	CFGVAR(Menu_Text_Active, Color_t({ 220, 220, 220, 255 }));
	CFGVAR(Menu_Text_Inactive, Color_t({ 160, 160, 160, 255 }));
	CFGVAR(Menu_Text_Disabled, Color_t({ 100, 100, 100, 255 }));

	CFGVAR(Menu_Accent_Primary, Color_t({ 0, 100, 120, 255 }));
	CFGVAR(Menu_Accent_Secondary, Color_t({ 0, 100, 120, 255 }));
	CFGVAR(Menu_Background, Color_t({ 0, 20, 20, 255 }));

	CFGVAR(Menu_Snow, false);

#pragma endregion
}