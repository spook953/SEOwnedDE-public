#pragma once

#include "../../TF2/IMatSystemSurface.h"

enum class EFonts
{
	Menu,
	ESP, ESP_CONDS, ESP_SMALL
};

class CFont
{
public:
	const char *m_szName;
	int m_nTall, m_nFlags, m_nWeight;
	DWORD m_dwFont;
};

class CFontManager
{
private:
	std::map<EFonts, CFont> m_mapFonts = {};

public:
	void Reload();
	const CFont &Get(EFonts eFont);
};

MAKE_SINGLETON_SCOPED(CFontManager, Fonts, H);