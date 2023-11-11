#pragma once

#include "../../../../SDK/SDK.h"

struct ClipRegion_t { int x = 0, y = 0, w = 0, h = 0; };

class CLateRenderer
{
private:
	struct String_t
	{
		CFont m_dwFont;
		int m_nX, m_nY;
		Color_t m_Color;
		short m_nAlign;
		const char *m_pwszString;
		ClipRegion_t m_Clip;
	};

	struct Rect_t
	{
		int m_nX, m_nY, m_nW, m_nH;
		Color_t m_Color;
	};

	struct OutlinedRect_t
	{
		int m_nX, m_nY, m_nW, m_nH;
		Color_t m_Color;
	};

	struct Texture_t
	{
		int m_nID;
		int m_nX, m_nY, m_nW, m_nH;
	};

	std::vector<String_t> m_vecStrings = {};
	std::vector<Rect_t> m_vecOutlinedRects = {};
	std::vector<Rect_t> m_vecRects = {};
	std::vector<Texture_t> m_vecTextures = {};

public:
	void String(const CFont &font, int x, int y, Color_t col, short align, const char *str, const ClipRegion_t &clip);
	void Rect(int x, int y, int w, int h, Color_t col);
	void OutlinedRect(int x, int y, int w, int h, Color_t col);
	void Texture(int id, int x, int y, int w, int h);

	void Clear();
	void DrawAll();
};

MAKE_SINGLETON_SCOPED(CLateRenderer, LateRender, H)