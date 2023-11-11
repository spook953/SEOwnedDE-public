#pragma once

#include "../Fonts/Fonts.h"

#define POS_DEFAULT (1 << 0)
#define POS_LEFT (1 << 1)
#define POS_TOP (1 << 2)
#define POS_CENTERX (1 << 3)
#define POS_CENTERY (1 << 4)
#define POS_CENTERXY POS_CENTERX | POS_CENTERY

class CDraw
{
private:
	int m_nScreenW = 0, m_nScreenH = 0;
	VMatrix m_WorldToProjection = {};

public:
	void UpdateScreenSize();
	void UpdateW2SMatrix();

	inline int GetScreenW() { return m_nScreenW; }
	inline int GetScreenH() { return m_nScreenH; }

public:
	bool W2S(const Vec3 &vOrigin, Vec3 &vScreen);
	bool ClipTransformWithProjection(const matrix3x4_t &worldToScreen, const Vec3 &point, Vec3 *pClip);
	bool ClipTransform(const Vector &point, Vector *pClip);
	bool ScreenPosition(const Vec3 &vPoint, Vec3 &vScreen);
	void String(const CFont &font, int x, int y, Color_t clr, short pos, const char *str, ...);
	void String(const CFont &font, int x, int y, Color_t clr, short pos, const wchar_t *str, ...);
	void Line(int x, int y, int x1, int y1, Color_t clr);
	void Rect(int x, int y, int w, int h, Color_t clr);
	void OutlinedRect(int x, int y, int w, int h, Color_t clr);
	void GradientRect(int x, int y, int w, int h, Color_t top_clr, Color_t bottom_clr, bool horizontal);
	void OutlinedCircle(int x, int y, int radius, int segments, Color_t clr);
	void FilledCircle(int x, int y, int radius, int segments, Color_t clr);
	void Texture(int x, int y, int w, int h, int id, short pos);
	void Polygon(int count, Vertex_t *vertices, Color_t clr);
	void FilledTriangle(const std::array<Vec2, 3> &points, Color_t clr);
	void Arc(int x, int y, int radius, float thickness, float start, float end, Color_t col);
	void StartClipping(int x, int y, int w, int h);
	void EndClipping();
	void FillRectRounded(int x, int y, int w, int h, int radius, Color_t col);
};

MAKE_SINGLETON_SCOPED(CDraw, Draw, H);