#pragma once

#include "../../../SDK/SDK.h"

class CESP
{
private:
	bool GetDrawBounds(C_BaseEntity *pEntity, int &x, int &y, int &w, int &h);
	void DrawBones(C_TFPlayer *pPlayer, Color_t Color);

public:
	void Run();
};

MAKE_SINGLETON_SCOPED(CESP, ESP, F);