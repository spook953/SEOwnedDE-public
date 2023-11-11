#include "Fonts.h"

void CFontManager::Reload()
{
	m_mapFonts[EFonts::Menu] = { "Verdana", 12, FONTFLAG_ANTIALIAS, 0 };
	m_mapFonts[EFonts::ESP] = { "Verdana", 12, FONTFLAG_OUTLINE, 0 };
	m_mapFonts[EFonts::ESP_CONDS] = { "Small Fonts", 9, FONTFLAG_OUTLINE, 0 };
	m_mapFonts[EFonts::ESP_SMALL] = { "Small Fonts", 11, FONTFLAG_OUTLINE, 0 };

	for (auto &v : m_mapFonts)
	{
		I::MatSystemSurface->SetFontGlyphSet
		(
			v.second.m_dwFont = I::MatSystemSurface->CreateFont(),
			v.second.m_szName,	//name
			v.second.m_nTall,	//tall
			v.second.m_nWeight,	//weight
			0,					//blur
			0,					//scanlines
			v.second.m_nFlags	//flags
		);
	}
}

const CFont &CFontManager::Get(EFonts eFont)
{
	return m_mapFonts[eFont];
}