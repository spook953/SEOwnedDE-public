#include "Draw.h"
#include "../../TF2/cdll_int.h"
#include "../../TF2/ivrenderview.h"

#pragma warning (disable : 6385)

void CDraw::UpdateScreenSize()
{
	m_nScreenW = I::BaseClientDLL->GetScreenWidth();
	m_nScreenH = I::BaseClientDLL->GetScreenHeight();
}

void CDraw::UpdateW2SMatrix()
{
	CViewSetup ViewSetup = {};

	if (I::BaseClientDLL->GetPlayerView(ViewSetup))
	{
		static VMatrix WorldToView = {};
		static VMatrix ViewToProjection = {};
		static VMatrix WorldToPixels = {};

		I::RenderView->GetMatricesForView(ViewSetup, &WorldToView, &ViewToProjection, &m_WorldToProjection, &WorldToPixels);
	}
}

bool CDraw::W2S(const Vec3 &vOrigin, Vec3 &vScreen)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warray-bounds"

	const matrix3x4_t &w2s = m_WorldToProjection.As3x4();

	float w = w2s[3][0] * vOrigin[0] + w2s[3][1] * vOrigin[1] + w2s[3][2] * vOrigin[2] + w2s[3][3];

	if (w > 0.001f)
	{
		float flsw = static_cast<float>(m_nScreenW);
		float flsh = static_cast<float>(m_nScreenH);
		float fl1dbw = 1.0f / w;

		vScreen.x = (flsw / 2.0f) + (0.5f * ((w2s[0][0] * vOrigin[0] + w2s[0][1] * vOrigin[1] + w2s[0][2] * vOrigin[2] + w2s[0][3]) * fl1dbw) * flsw + 0.5f);
		vScreen.y = (flsh / 2.0f) - (0.5f * ((w2s[1][0] * vOrigin[0] + w2s[1][1] * vOrigin[1] + w2s[1][2] * vOrigin[2] + w2s[1][3]) * fl1dbw) * flsh + 0.5f);

		return true;
	}

	return false;

#pragma clang diagnostic pop
}

bool CDraw::ClipTransformWithProjection(const matrix3x4_t &worldToScreen, const Vec3 &point, Vec3 *pClip)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warray-bounds"

	pClip->x = worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3];
	pClip->y = worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3];
	pClip->z = 0.0f;

	float w = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];

	bool behind = w < 0.001f;

	if (behind)
	{
		pClip->x *= 100000.0f;
		pClip->y *= 100000.0f;
	}

	else
	{
		float invw = 1.0f / w;
		pClip->x *= invw;
		pClip->y *= invw;
	}

	return behind;

#pragma clang diagnostic pop
}

bool CDraw::ClipTransform(const Vector &point, Vector *pClip)
{
	const matrix3x4_t &worldToScreen = m_WorldToProjection.As3x4();
	return ClipTransformWithProjection(worldToScreen, point, pClip);
}

bool CDraw::ScreenPosition(const Vec3 &vPoint, Vec3 &vScreen)
{
	int retval = ClipTransform(vPoint, &vScreen);
	int w = m_nScreenW, h = m_nScreenH;
	vScreen.x = (0.5f * vScreen.x * w) + 0.5f * w;
	vScreen.y = (-0.5f * vScreen.y * h) + 0.5f * h;
	return retval;
}

void CDraw::String(const CFont &font, int x, int y, Color_t clr, short pos, const char *str, ...)
{
	if (!str)
		return;

	va_list va_alist = {};
	char cbuffer[1024] = { '\0' };
	wchar_t wstr[1024] = { '\0' };

	va_start(va_alist, str);
	vsprintf_s(cbuffer, str, va_alist);
	va_end(va_alist);
	wsprintfW(wstr, L"%hs", cbuffer);

	if (pos)
	{
		int w = 0, h = 0;
		I::MatSystemSurface->GetTextSize(font.m_dwFont, wstr, w, h);

		if (pos & POS_LEFT) { x -= w; }
		if (pos & POS_TOP) { y -= h; }
		if (pos & POS_CENTERX) { x -= (w / 2); }
		if (pos & POS_CENTERY) { y -= (h / 2); }
	}

	I::MatSystemSurface->DrawSetTextPos(x, y);
	I::MatSystemSurface->DrawSetTextFont(font.m_dwFont);
	I::MatSystemSurface->DrawSetTextColor(clr.r, clr.g, clr.b, clr.a);
	I::MatSystemSurface->DrawPrintText(wstr, wcslen(wstr));
}

void CDraw::String(const CFont &font, int x, int y, Color_t clr, short pos, const wchar_t *str, ...)
{
	if (!str)
		return;

	va_list va_alist{};
	wchar_t wstr[1024] = { '\0' };

	va_start(va_alist, str);
	vswprintf_s(wstr, str, va_alist);
	va_end(va_alist);

	if (pos)
	{
		int w = 0, h = 0;
		I::MatSystemSurface->GetTextSize(font.m_dwFont, wstr, w, h);

		if (pos & POS_LEFT) { x -= w; }
		if (pos & POS_TOP) { y -= h; }
		if (pos & POS_CENTERX) { x -= (w / 2); }
		if (pos & POS_CENTERY) { y -= (h / 2); }
	}

	I::MatSystemSurface->DrawSetTextPos(x, y);
	I::MatSystemSurface->DrawSetTextFont(font.m_dwFont);
	I::MatSystemSurface->DrawSetTextColor(clr.r, clr.g, clr.b, clr.a);
	I::MatSystemSurface->DrawPrintText(wstr, wcslen(wstr));
}

void CDraw::Line(int x, int y, int x1, int y1, Color_t clr)
{
	I::MatSystemSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::MatSystemSurface->DrawLine(x, y, x1, y1);
}

void CDraw::Rect(int x, int y, int w, int h, Color_t clr)
{
	I::MatSystemSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::MatSystemSurface->DrawFilledRect(x, y, x + w, y + h);
}

void CDraw::OutlinedRect(int x, int y, int w, int h, Color_t clr)
{
	I::MatSystemSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::MatSystemSurface->DrawOutlinedRect(x, y, x + w, y + h);
}

void CDraw::GradientRect(int x, int y, int w, int h, Color_t top_clr, Color_t bottom_clr, bool horizontal)
{
	I::MatSystemSurface->DrawSetColor(top_clr.r, top_clr.g, top_clr.b, top_clr.a);
	I::MatSystemSurface->DrawFilledRectFade(x, y, x + w, y + h, 255, 255, horizontal);

	I::MatSystemSurface->DrawSetColor(bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a);
	I::MatSystemSurface->DrawFilledRectFade(x, y, x + w, y + h, 0, 255, horizontal);
}

void CDraw::OutlinedCircle(int x, int y, int radius, int segments, Color_t clr)
{
	I::MatSystemSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::MatSystemSurface->DrawOutlinedCircle(x, y, radius, segments);
}

void CDraw::FilledCircle(int x, int y, int radius, int segments, Color_t clr)
{
	static std::vector<Vertex_t> vertices = {};

	const float step = static_cast<float>(PI) * 2.0f / segments;

	for (float a = 0; a < PI * 2.0f; a += step)
		vertices.emplace_back(Vertex_t{ { radius * cosf(a) + x, radius * sinf(a) + y } });

	Polygon(segments, vertices.data(), clr);
	
	vertices.clear();
}

void CDraw::Texture(int x, int y, int w, int h, int id, short pos)
{
	if (pos)
	{
		if (pos & POS_LEFT)
			x -= w;

		if (pos & POS_TOP)
			y -= h;

		if (pos & POS_CENTERX)
			x -= (w / 2);

		if (pos & POS_CENTERY)
			y -= (h / 2);
	}

	I::MatSystemSurface->DrawSetTexture(id);
	I::MatSystemSurface->DrawSetColor(255, 255, 255, 255);
	I::MatSystemSurface->DrawTexturedRect(x, y, x + w, y + h);
}

void CDraw::Polygon(int count, Vertex_t *vertices, Color_t clr)
{
	static int id = 0;

	if (!I::MatSystemSurface->IsTextureIDValid(id))
		id = I::MatSystemSurface->CreateNewTextureID();

	I::MatSystemSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::MatSystemSurface->DrawSetTexture(id);
	I::MatSystemSurface->DrawTexturedPolygon(count, vertices);
}

void CDraw::FilledTriangle(const std::array<Vec2, 3> &points, Color_t clr)
{
	std::array<Vertex_t, 3> vertices = { Vertex_t{ { points[0] } }, Vertex_t{ { points[1] } }, Vertex_t{ { points[2] } } };
	Polygon(3, vertices.data(), clr);
}

void CDraw::Arc(int x, int y, int radius, float thickness, float start, float end, Color_t clr)
{
	static const float flPercision = 7.2f;
	static const float flStep = static_cast<float>(PI / 180.0f);

	float flInner = radius - thickness;

	for (float flAngle = start; flAngle < start + end; flAngle += flPercision)
	{
		float flRad = flAngle * flStep;
		float flRad2 = (flAngle + flPercision) * flStep;

		float flRadCos = std::cosf(flRad);
		float flRadSin = std::sinf(flRad);
		float flRad2Cos = std::cosf(flRad2);
		float flRad2Sin = std::sinf(flRad2);

		Vec2 vecInner1 = { x + flRadCos * flInner, y + flRadSin * flInner };
		Vec2 vecInner2 = { x + flRad2Cos * flInner, y + flRad2Sin * flInner };

		Vec2 vecOuter1 = { x + flRadCos * radius, y + flRadSin * radius };
		Vec2 vecOuter2 = { x + flRad2Cos * radius, y + flRad2Sin * radius };

		Vertex_t polys1[3] = { Vertex_t{ vecOuter1 }, Vertex_t{ vecOuter2 }, Vertex_t{ vecInner1 } };
		Polygon(sizeof(polys1) / sizeof(Vertex_t), polys1, clr);

		Vertex_t polys2[3] = { Vertex_t{ vecInner1 }, Vertex_t{ vecOuter2 }, Vertex_t{ vecInner2 } };
		Polygon(sizeof(polys2) / sizeof(Vertex_t), polys2, clr);
	}
}

void CDraw::StartClipping(int x, int y, int w, int h)
{
	I::MatSystemSurface->DisableClipping(false);
	I::MatSystemSurface->SetClippingRect(x, y, x + w, y + h);
}

void CDraw::EndClipping()
{
	I::MatSystemSurface->DisableClipping(true);
}

void CDraw::FillRectRounded(int x, int y, int w, int h, int radius, Color_t col)
{
	Vertex_t round[64] = {};

	for (int i = 0; i < 4; i++)
	{
		int _x = x + ((i < 2) ? (w - radius) : radius);
		int _y = y + ((i % 3) ? (h - radius) : radius);

		float a = 90.0f * i;

		for (int j = 0; j < 16; j++) {
			float _a = DEG2RAD(a + j * 6.0f);
			round[(i * 16) + j] = Vertex_t(Vector2D(_x + radius * sinf(_a), _y - radius * cosf(_a)));
		}
	}

	Polygon(64, round, col);
}