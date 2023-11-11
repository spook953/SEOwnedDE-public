#pragma once

#include "../../../SDK/SDK.h"

class CVisualUtils
{
public:
	bool IsEntityOwnedBy(C_BaseEntity *pEntity, C_BaseEntity *pWho);

	Color_t GetEntityColor(C_TFPlayer *pLocal, C_BaseEntity *pEntity);
	Color_t GetHealthColor(int nHealth, int nMaxHealth);
	Color_t GetHealthColorAlt(int nHealth, int nMaxHealth);

	int CreateTextureFromArray(const unsigned char *rgba, int w, int h);
	int CreateTextureFromVTF(const char *name);

	int GetClassIcon(int nClassNum);
	int GetBuildingTextureId(C_BaseObject *pObject);
	int GetHealthIconTextureId();
	int GetAmmoIconTextureId();
	int GetHalloweenGiftTextureId();

	bool IsOnScreen(C_TFPlayer *pLocal, C_BaseEntity *pEntity);
	bool IsOnScreenNoEntity(C_TFPlayer *pLocal, const Vec3 &vAbsOrigin);

	int GetCat(int nFrame);
	int GetCat2(int nFrame);
	int GetCatSleep(int nFrame);
	int GetCatRun(int nFrame);

	Color_t Rainbow();
	Color_t RainbowTickOffset(int nTick);
};

MAKE_SINGLETON_SCOPED(CVisualUtils, VisualUtils, F)