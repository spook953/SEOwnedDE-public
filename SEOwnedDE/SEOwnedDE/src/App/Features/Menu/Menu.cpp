#include "Menu.h"

#include "LateRenderer/LateRenderer.h"

#include "../CFG.h"
#include "../VisualUtils/VisualUtils.h"
#include "../Players/Players.h"

#define multiselect(label, unique, ...) static std::vector<std::pair<const char *, bool &>> unique##multiselect = __VA_ARGS__; \
SelectMulti(label, unique##multiselect)

void CMenu::Drag(int &x, int &y, int w, int h, int offset_y)
{
	static POINT delta = {};
	static bool drag = false;
	static bool move = false;

	static bool held = false;

	if (!H::Input->IsPressed(VK_LBUTTON) && !H::Input->IsHeld(VK_LBUTTON))
		held = false;

	int mousex = H::Input->GetMouseX();
	int mousey = H::Input->GetMouseY();

	if ((mousex > x && mousex < x + w && mousey > y - offset_y && mousey < y - offset_y + h) && (held || H::Input->IsPressed(VK_LBUTTON)))
	{
		held = true;
		drag = true;

		if (!move)
		{
			delta.x = mousex - x;
			delta.y = mousey - y;
			move = true;
		}
	}

	if (drag)
	{
		x = mousex - delta.x;
		y = mousey - delta.y;
	}

	if (!held)
	{
		drag = false;
		move = false;
	}
}

bool CMenu::IsHovered(int x, int y, int w, int h, void *pVar, bool bStrict)
{
	//this is pretty ok to use but like.. it can have annoying visual bugs with clicks..
	/*if (H::Input->IsHeld(VK_LBUTTON))
		return false;*/

	if (pVar == nullptr)
	{
		for (const auto &State : m_mapStates)
		{
			if (State.second)
				return false;
		}
	}

	else
	{
		for (const auto &State : m_mapStates)
		{
			if (State.second && State.first != pVar)
				return false;
		}
	}

	int mx = H::Input->GetMouseX();
	int my = H::Input->GetMouseY();

	if (bStrict)
	{
		bool bLeft = mx >= x;
		bool bRight = mx <= x + w;
		bool bTop = my >= y;
		bool bBottom = my <= y + h;

		return bLeft && bRight && bTop && bBottom;
	}

	else
	{
		bool bLeft = mx > x;
		bool bRight = mx < x + w;
		bool bTop = my > y;
		bool bBottom = my < y + h;

		return bLeft && bRight && bTop && bBottom;
	}
}

bool CMenu::IsHoveredSimple(int x, int y, int w, int h)
{
	int mx = H::Input->GetMouseX();
	int my = H::Input->GetMouseY();

	bool bLeft = mx >= x;
	bool bRight = mx <= x + w;
	bool bTop = my >= y;
	bool bBottom = my <= y + h;

	return bLeft && bRight && bTop && bBottom;
}

void CMenu::GroupBoxStart(const char *szLabel, int nWidth)
{
	m_nCursorY += CFG::Menu_Spacing_Y * 2; //hmm

	m_nLastGroupBoxY = m_nCursorY;
	m_nLastGroupBoxW = nWidth;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = nWidth;

	int nTextW = [&]() -> int {
		int w_out = 0, h_out = 0;
		I::MatSystemSurface->GetTextSize(H::Fonts->Get(EFonts::Menu).m_dwFont, Utils::ConvertUtf8ToWide(szLabel).c_str(), w_out, h_out);
		return w_out;
	}();

	int nWidthRemaining = w - (nTextW + (CFG::Menu_Spacing_X * 4));
	int nSideWidth = nWidthRemaining / 2;

	H::Draw->Line(x, y, x + nSideWidth, y, CFG::Menu_Accent_Primary);
	H::Draw->Line(x + w, y, x + (w - nSideWidth), y, CFG::Menu_Accent_Primary);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x + (w / 2), y - (CFG::Menu_Spacing_Y - 1), CFG::Menu_Text_Inactive, POS_CENTERXY, szLabel
	);

	m_nCursorX += CFG::Menu_Spacing_X * 2;
	m_nCursorY += CFG::Menu_Spacing_Y * 2;
}

void CMenu::GroupBoxEnd()
{
	m_nCursorX -= (CFG::Menu_Spacing_X * 2);
	m_nCursorY += 2;

	Color_t clr = CFG::Menu_Accent_Primary;

	H::Draw->Line(m_nCursorX, m_nLastGroupBoxY, m_nCursorX, m_nCursorY, clr);
	H::Draw->Line(m_nCursorX + m_nLastGroupBoxW, m_nLastGroupBoxY, m_nCursorX + m_nLastGroupBoxW, m_nCursorY, clr);
	H::Draw->Line(m_nCursorX, m_nCursorY, m_nCursorX + m_nLastGroupBoxW, m_nCursorY, clr);

	m_nCursorY += CFG::Menu_Spacing_Y;
}

bool CMenu::CheckBox(const char *szLabel, bool &bVar)
{
	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = CFG::Menu_CheckBox_Width;
	int h = CFG::Menu_CheckBox_Height;

	int w_with_text = [&]() -> int {
		int w_out = 0, h_out = 0;
		I::MatSystemSurface->GetTextSize(H::Fonts->Get(EFonts::Menu).m_dwFont, Utils::ConvertUtf8ToWide(szLabel).c_str(), w_out, h_out);
		return w + w_out + 1;
	}();

	bool bHovered = IsHovered(x, y, w_with_text, h, &bVar);

	if (bHovered && H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed) {
		bCallback = m_bClickConsumed = true;
		bVar = !bVar;
	}

	if (bVar) {
		Color_t clr = CFG::Menu_Accent_Primary;
		H::Draw->GradientRect(x, y, w, h, { clr.r, clr.g, clr.b, 25 }, clr, false);
	}

	H::Draw->OutlinedRect(
		x, y, w, h, CFG::Menu_Accent_Primary
	);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x + w + CFG::Menu_Spacing_X,
		y + (h / 2),
		(bHovered) ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_CENTERY,
		szLabel
	);

	m_nCursorY += h + CFG::Menu_Spacing_Y;

	return bCallback;
}

bool CMenu::SliderFloat(const char *szLabel, float &flVar, float flMin, float flMax, float flStep, const char *szFormat)
{
	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = CFG::Menu_Slider_Width;
	int h = CFG::Menu_Slider_Height;

	int nTextH = H::Fonts->Get(EFonts::Menu).m_nTall;

	bool bHovered = IsHovered(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, &flVar, true);

	bool bAcceptsInput = [&]() -> bool
	{
		for (const auto &State : m_mapStates)
		{
			if (State.second && State.first != &flVar)
				return false;
		}

		return true;
	}();

	if (!m_bClickConsumed && bAcceptsInput)
	{
		if (H::Input->IsPressed(VK_RBUTTON))
		{
			if (bHovered)
			{
				bool bLeftSideHovered = IsHovered(x, y + (nTextH + CFG::Menu_Spacing_Y), w / 2, h, &flVar, true);

				if (bLeftSideHovered)
					flVar -= flStep;

				else flVar += flStep;
			}
		}

		if (H::Input->IsPressed(VK_LBUTTON))
		{
			if (bHovered) {
				m_bClickConsumed = true;
				m_mapStates[&flVar] = true;
			}
		}

		else
		{
			if (!H::Input->IsHeld(VK_LBUTTON))
				m_mapStates[&flVar] = false;
		}
	}

	if (m_mapStates[&flVar])
	{
		flVar = Math::RemapValClamped(
			static_cast<float>(H::Input->GetMouseX()),
			static_cast<float>(x), static_cast<float>(x + w),
			flMin, flMax
		);
	}

	if (flVar < 0.0f)
		flVar = flVar - fmodf((flVar - flStep / 2.0f), flStep) - flStep / 2.0f;

	else flVar = flVar - fmodf((flVar + flStep / 2.0f), flStep) + flStep / 2.0f;

	flVar = static_cast<float>(static_cast<int>(flVar * 100.0f + 0.5f)) / 100.0f;
	flVar = std::clamp(flVar, flMin, flMax);

	int nFillWidth = static_cast<int>(Math::RemapValClamped(
		flVar,
		flMin, flMax,
		0.0f, static_cast<float>(w)
	));

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x, y,
		(bHovered || m_mapStates[&flVar]) ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_DEFAULT,
		szLabel
	);

	Color_t clr = CFG::Menu_Accent_Primary;
	Color_t clr_dim = { clr.r, clr.g, clr.b, 25 };

	H::Draw->Rect(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, clr_dim);
	H::Draw->GradientRect(x, y + (nTextH + CFG::Menu_Spacing_Y), nFillWidth, h, clr_dim, clr, false);
	H::Draw->OutlinedRect(x, y + (nTextH + CFG::Menu_Spacing_Y), nFillWidth, h, clr);
	H::Draw->Rect(x + (nFillWidth - 1), y + (nTextH + CFG::Menu_Spacing_Y) - 1, 2, h + 2, CFG::Menu_Text_Active);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x + (w + CFG::Menu_Spacing_X),
		y + (nTextH - 1),
		(bHovered || m_mapStates[&flVar]) ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_DEFAULT,
		szFormat, flVar
	);

	m_nCursorY += h + nTextH + CFG::Menu_Spacing_Y + CFG::Menu_Spacing_Y + 2; //+ 2 for the little white thingy

	return bCallback;
}

bool CMenu::SliderInt(const char *szLabel, int &nVar, int nMin, int nMax, int nStep)
{
	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = CFG::Menu_Slider_Width;
	int h = CFG::Menu_Slider_Height;

	int nTextH = H::Fonts->Get(EFonts::Menu).m_nTall;

	bool bHovered = IsHovered(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, &nVar, true);

	bool bAcceptsInput = [&]() -> bool
	{
		for (const auto &State : m_mapStates)
		{
			if (State.second && State.first != &nVar)
				return false;
		}

		return true;
	}();

	if (!m_bClickConsumed && bAcceptsInput)
	{
		if (H::Input->IsPressed(VK_RBUTTON))
		{
			if (bHovered)
			{
				bool bLeftSideHovered = IsHovered(x, y + (nTextH + CFG::Menu_Spacing_Y), w / 2, h, &nVar, true);

				if (bLeftSideHovered)
					nVar -= nStep;

				else nVar += nStep;
			}
		}

		if (H::Input->IsPressed(VK_LBUTTON))
		{
			if (bHovered) {
				m_bClickConsumed = true;
				m_mapStates[&nVar] = true;
			}
		}

		else
		{
			if (!H::Input->IsHeld(VK_LBUTTON))
				m_mapStates[&nVar] = false;
		}
	}

	if (m_mapStates[&nVar])
	{
		nVar = static_cast<int>(Math::RemapValClamped(
			static_cast<float>(H::Input->GetMouseX()),
			static_cast<float>(x), static_cast<float>(x + w),
			static_cast<float>(nMin), static_cast<float>(nMax)
		));
	}

	if (nVar < 0)
		nVar = nVar - (nVar - nStep / 2) % nStep - nStep / 2;

	else nVar = nVar - (nVar + nStep / 2) % nStep + nStep / 2;

	nVar = std::clamp(nVar, nMin, nMax);

	int nFillWidth = static_cast<int>(Math::RemapValClamped(
		static_cast<float>(nVar),
		static_cast<float>(nMin), static_cast<float>(nMax),
		0.0f, static_cast<float>(w)
	));

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x, y,
		(bHovered || m_mapStates[&nVar]) ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_DEFAULT,
		szLabel
	);

	Color_t clr = CFG::Menu_Accent_Primary;
	Color_t clr_dim = { clr.r, clr.g, clr.b, 25 };

	H::Draw->Rect(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, clr_dim);
	H::Draw->GradientRect(x, y + (nTextH + CFG::Menu_Spacing_Y), nFillWidth, h, clr_dim, clr, false);
	H::Draw->OutlinedRect(x, y + (nTextH + CFG::Menu_Spacing_Y), nFillWidth, h, clr);
	H::Draw->Rect(x + (nFillWidth - 1), y + (nTextH + CFG::Menu_Spacing_Y) - 1, 2, h + 2, CFG::Menu_Text_Active);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x + (w + CFG::Menu_Spacing_X),
		y + (nTextH - 1),
		(bHovered || m_mapStates[&nVar]) ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_DEFAULT,
		"%d", nVar
	);

	m_nCursorY += h + nTextH + CFG::Menu_Spacing_Y + CFG::Menu_Spacing_Y + 2;

	return bCallback;
}

bool CMenu::InputKey(const char *szLabel, int &nKeyOut)
{
	auto VK2STR = [&](const short key) -> std::string
	{
		switch (key)
		{
			case VK_LBUTTON: return "LButton";
			case VK_RBUTTON: return "RButton";
			case VK_MBUTTON: return "MButton";
			case VK_XBUTTON1: return "XButton1";
			case VK_XBUTTON2: return "XButton2";
			case VK_NUMPAD0: return "NumPad0";
			case VK_NUMPAD1: return "NumPad1";
			case VK_NUMPAD2: return "NumPad2";
			case VK_NUMPAD3: return "NumPad3";
			case VK_NUMPAD4: return "NumPad4";
			case VK_NUMPAD5: return "NumPad5";
			case VK_NUMPAD6: return "NumPad6";
			case VK_NUMPAD7: return "NumPad7";
			case VK_NUMPAD8: return "NumPad8";
			case VK_NUMPAD9: return "NumPad9";
			case VK_MENU: return "Alt";
			case VK_CAPITAL: return "Caps Lock";
			case 0x0: return "None";
			default: break;
		}

		CHAR output[16] = { "\0" };

		if (const int result = GetKeyNameTextA(MapVirtualKeyW(key, MAPVK_VK_TO_VSC) << 16, output, 16))
			return output;

		return "VK2STR_FAILED";
	};

	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = CFG::Menu_InputKey_Width;
	int h = CFG::Menu_InputKey_Height;

	int w_with_text = [&]() -> int {
		int w_out = 0, h_out = 0;
		I::MatSystemSurface->GetTextSize(H::Fonts->Get(EFonts::Menu).m_dwFont, Utils::ConvertUtf8ToWide(szLabel).c_str(), w_out, h_out);
		return w + w_out + 1;
	}();

	bool bHovered = IsHovered(x, y, w_with_text, h, &nKeyOut);
	bool bActive = m_mapStates[&nKeyOut] || bHovered;

	if (!m_mapStates[&nKeyOut] && bHovered && H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed)
		m_mapStates[&nKeyOut] = m_bClickConsumed = true;

	if (m_mapStates[&nKeyOut])
	{
		for (int n = 0; n < 256; n++)
		{
			bool bMouse = (n > 0x0 && n < 0x7);
			bool bLetter = (n > L'A' - 1 && n < L'Z' + 1);
			bool bAllowed = (n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT || n == VK_ESCAPE || n == VK_INSERT || n == VK_MENU || n == VK_CAPITAL || n == VK_SPACE || n == VK_CONTROL);
			bool bNumPad = n > (VK_NUMPAD0 - 1) && n < (VK_NUMPAD9)+1;

			if (bMouse || bLetter || bAllowed || bNumPad)
			{
				if (H::Input->IsPressed(n))
				{
					if (n == VK_INSERT) {
						m_mapStates[&nKeyOut] = false;
						break;
					}

					else if (n == VK_ESCAPE) {
						nKeyOut = 0x0;
						m_mapStates[&nKeyOut] = false;
						break;
					}

					else
					{
						if (n == VK_LBUTTON)
						{
							if (m_bClickConsumed)
								continue;

							m_bClickConsumed = true;
						}

						nKeyOut = n;
						m_mapStates[&nKeyOut] = false;
					}

					break;
				}
			}
		}
	}

	Color_t clr = CFG::Menu_Accent_Primary;

	if (bActive)
		H::Draw->Rect(x, y, w, h, { clr.r, clr.g, clr.b, 25 });

	H::Draw->OutlinedRect(x, y, w, h, clr);

	if (m_mapStates[&nKeyOut])
	{
		H::Draw->String(
			H::Fonts->Get(EFonts::Menu),
			x + (w / 2),
			y + (h / 2),
			bActive ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
			POS_CENTERXY,
			"...");
	}

	else
	{
		H::Draw->String(
			H::Fonts->Get(EFonts::Menu),
			x + (w / 2),
			y + (h / 2),
			bActive ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
			POS_CENTERXY,
			VK2STR(nKeyOut).c_str());
	}

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x + (w + CFG::Menu_Spacing_X),
		y + (h / 2),
		bActive ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_CENTERY,
		szLabel);

	m_nCursorY += h + CFG::Menu_Spacing_Y;

	return bCallback;
}

bool CMenu::Button(const char *szLabel, bool bActive, int nCustomWidth)
{
	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = 0;
	int h = 0;

	I::MatSystemSurface->GetTextSize(H::Fonts->Get(EFonts::Menu).m_dwFont, Utils::ConvertUtf8ToWide(szLabel).c_str(), w, h);

	if (!w || !h)
		return false;

	if (nCustomWidth > 0)
		w = nCustomWidth;

	w += CFG::Menu_Spacing_X * 2;
	h += CFG::Menu_Spacing_Y - 1;

	bool bHovered = IsHovered(x, y, w, h, nullptr);

	if (bHovered && H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed)
		bCallback = m_bClickConsumed = true;

	Color_t clr = CFG::Menu_Accent_Primary;
	Color_t clr_dim = { clr.r, clr.g, clr.b, (bHovered || bActive) ? static_cast<byte>(50) : static_cast<byte>(0) };

	H::Draw->Rect(x, y, w, h, clr_dim);
	H::Draw->OutlinedRect(x, y, w, h, clr);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x + (w / 2), y + (h / 2) - 1,
		(bHovered || bActive) ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_CENTERXY, szLabel
	);

	m_nCursorY += h + CFG::Menu_Spacing_Y;
	m_nLastButtonW = w;

	return bCallback;
}

bool CMenu::playerListButton(const wchar_t *label, int nCustomWidth, Color_t clr, bool center_txt)
{
	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = nCustomWidth;
	int h = H::Fonts->Get(EFonts::Menu).m_nTall;

	w += CFG::Menu_Spacing_X * 2;
	h += CFG::Menu_Spacing_Y - 1;

	bool bHovered = IsHovered(x, y, w, h, nullptr);

	if (bHovered && H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed)
		bCallback = m_bClickConsumed = true;

	Color_t clrA = CFG::Menu_Accent_Primary;
	Color_t clr_dim = { clrA.r, clrA.g, clrA.b, bHovered ? static_cast<byte>(50) : static_cast<byte>(0) };

	H::Draw->Rect(x, y, w, h, clr_dim);
	H::Draw->OutlinedRect(x, y, w, h, clrA);

	H::Draw->StartClipping(x, y, w, h);

	if (center_txt)
	{
		H::Draw->String
		(
			H::Fonts->Get(EFonts::Menu),
			x + (w / 2), y + (h / 2) - 1,
			clr,
			POS_CENTERXY, label
		);
	}

	else
	{
		H::Draw->String
		(
			H::Fonts->Get(EFonts::Menu),
			x + CFG::Menu_Spacing_X, y + (h / 2) - 1,
			clr,
			POS_CENTERY, label
		);
	}

	H::Draw->EndClipping();

	m_nCursorY += h + CFG::Menu_Spacing_Y;
	m_nLastButtonW = w;

	return bCallback;
}

bool CMenu::InputText(const char *szLabel, const char *szLabel2, std::string &strOutput)
{
	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = 0;
	int h = 0;

	I::MatSystemSurface->GetTextSize(H::Fonts->Get(EFonts::Menu).m_dwFont, Utils::ConvertUtf8ToWide(szLabel).c_str(), w, h);

	if (!w || !h)
		return false;

	w += CFG::Menu_Spacing_X * 2;
	h += CFG::Menu_Spacing_Y - 1;

	bool bHovered = IsHovered(x, y, w, h, nullptr);

	Color_t clr = CFG::Menu_Accent_Primary;
	Color_t clr_dim = { clr.r, clr.g, clr.b, bHovered ? static_cast<byte>(50) : static_cast<byte>(0) };

	if (!m_mapStates[&strOutput])
	{
		H::Draw->Rect(x, y, w, h, clr_dim);
		H::Draw->OutlinedRect(x, y, w, h, clr);
		H::Draw->String(
			H::Fonts->Get(EFonts::Menu),
			x + (w / 2), y + (h / 2) - 1,
			bHovered ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
			POS_CENTERXY, szLabel
		);
	}

	bool bCanOpen = [&]() -> bool
	{
		for (const auto &State : m_mapStates)
		{
			if (State.second && State.first != &strOutput)
				return false;
		}

		return true;
	}();

	static std::string strTemp = {};

	if (bHovered && H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed && bCanOpen) {
		m_bClickConsumed = m_mapStates[&strOutput] = true;
		strTemp.clear();
	}

	if (H::Input->IsPressed(VK_ESCAPE) || H::Input->IsPressed(VK_INSERT)) {
		m_mapStates[&strOutput] = false;
		strTemp.clear();
	}

	if (m_mapStates[&strOutput])
	{
		y += CFG::Menu_Spacing_Y;

		int w = CFG::Menu_InputText_Width;
		int h = CFG::Menu_InputText_Height;

		H::LateRender->Rect(x, y, w, h, CFG::Menu_Background);
		H::LateRender->OutlinedRect(x, y, w, h, clr);
		H::LateRender->String(
			H::Fonts->Get(EFonts::Menu),
			x + CFG::Menu_Spacing_X,
			y + (CFG::Menu_Spacing_Y * 3),
			CFG::Menu_Text_Inactive,
			POS_CENTERY, szLabel2, {}
		);

		if (strTemp.length() < 15)
		{
			for (int n = 0; n < 256; n++)
			{
				if ((n > 'A' - 1 && n < 'Z' + 1) && H::Input->IsPressedAndHeld(n))
				{
					char ch = 0;

					if ((GetKeyState(VK_CAPITAL) & 1) || H::Input->IsHeld(VK_SHIFT))
						ch = static_cast<char>(n);

					else ch = static_cast<char>(std::tolower(n));

					strTemp += ch;
				}
			}
		}

		if (strTemp.length() > 0)
		{
			if (H::Input->IsPressedAndHeld(VK_BACK))
				strTemp.erase(strTemp.end() - 1);
		}

		if (H::Input->IsPressed(VK_RETURN)) {
			bCallback = strTemp.length() > 0;
			strOutput = std::string(strTemp.begin(), strTemp.end());
			m_mapStates[&strOutput] = false;
			strTemp.clear();
		}

		H::LateRender->String(
			H::Fonts->Get(EFonts::Menu),
			x + CFG::Menu_Spacing_X,
			y + (h - H::Fonts->Get(EFonts::Menu).m_nTall) + CFG::Menu_Spacing_Y,
			CFG::Menu_Text_Active,
			POS_CENTERY, strTemp.c_str(), {}
		);
	}

	m_nCursorY += h + CFG::Menu_Spacing_Y;

	m_nLastButtonW = w;

	return bCallback;
}

bool CMenu::SelectSingle(const char *szLabel, int &nVar, const std::vector<std::pair<const char *, int>> &vecSelects)
{
	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = CFG::Menu_Select_Width;
	int h = CFG::Menu_Select_Height;

	int nTextH = H::Fonts->Get(EFonts::Menu).m_nTall;

	bool bHovered = IsHovered(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, &nVar);
	bool bActive = bHovered || m_mapStates[&nVar];

	if (!m_bClickConsumed && bHovered && H::Input->IsPressed(VK_LBUTTON)) {
		m_mapStates[&nVar] = !m_mapStates[&nVar];
		m_bClickConsumed = true;
	}

	auto pszCurSelected = [&]() -> const char *
	{
		for (const auto &Select : vecSelects)
		{
			if (Select.second == nVar)
				return Select.first;
		}

		return "Unknown";
	}();

	Color_t clr = CFG::Menu_Accent_Primary;
	Color_t bg{ CFG::Menu_Background };
	Color_t clr_dim = { bg.r, bg.g, bg.b, 253 };

	H::Draw->Rect(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, clr_dim);

	if (!m_mapStates[&nVar])
		H::Draw->OutlinedRect(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, clr);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x + (w / 2), y + (h / 2) + (nTextH + CFG::Menu_Spacing_Y) - 1,
		bActive ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_CENTERXY,
		pszCurSelected
	);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x, y,
		(bHovered || m_mapStates[&nVar]) ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_DEFAULT,
		szLabel
	);

	if (m_mapStates[&nVar])
	{
		bool bSelectRegionHovered = IsHovered(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h * static_cast<int>(vecSelects.size()), &nVar);

		if (H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed && !bSelectRegionHovered) {
			m_bClickConsumed = true;
			m_mapStates[&nVar] = false;
		}
	}

	if (m_mapStates[&nVar])
	{
		H::LateRender->OutlinedRect(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h * static_cast<int>(vecSelects.size()), clr);

		int real_n{ 0 };

		for (int n = 0; n < static_cast<int>(vecSelects.size()); n++)
		{
			const auto &Select = vecSelects[n];

			if (Select.second == nVar)
			{
				continue;
			}

			int nSelectY = (y + h + (nTextH + CFG::Menu_Spacing_Y)) + (h * real_n);
			bool bSelectHovered = IsHovered(x, nSelectY, w, h, &nVar);

			H::LateRender->Rect(x, nSelectY, w, h, clr_dim);

			H::LateRender->String(
				H::Fonts->Get(EFonts::Menu),
				x + (w / 2), nSelectY,
				bSelectHovered ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
				POS_CENTERX,
				Select.first, {}
			);

			if (H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed && bSelectHovered) {
				nVar = Select.second;
				m_mapStates[&nVar] = false;
				m_bClickConsumed = true;
				break;
			}

			real_n++;
		}
	}

	m_nCursorY += h + nTextH + CFG::Menu_Spacing_Y + CFG::Menu_Spacing_Y;

	return bCallback;
}

bool CMenu::SelectMulti(const char *szLabel, std::vector<std::pair<const char *, bool &>> &vecSelects)
{
	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = CFG::Menu_Select_Width;
	int h = CFG::Menu_Select_Height;

	int nTextH = H::Fonts->Get(EFonts::Menu).m_nTall;

	bool bHovered = IsHovered(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, &vecSelects);
	bool bActive = bHovered || m_mapStates[&vecSelects];

	if (!m_bClickConsumed && bHovered && H::Input->IsPressed(VK_LBUTTON)) {
		m_mapStates[&vecSelects] = !m_mapStates[&vecSelects];
		m_bClickConsumed = true;
	}

	Color_t clr = CFG::Menu_Accent_Primary;
	Color_t bg{ CFG::Menu_Background };
	Color_t clr_dim = { bg.r, bg.g, bg.b, 253 };

	H::Draw->Rect(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, clr_dim);

	if (!m_mapStates[&vecSelects])
		H::Draw->OutlinedRect(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h, clr);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x, y,
		(bHovered || m_mapStates[&vecSelects]) ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_DEFAULT,
		szLabel
	);

	std::string strSelected = {};

	for (const auto &Select : vecSelects)
	{
		if (Select.second)
		{
			if (!strSelected.empty())
				strSelected += ", ";

			strSelected += Select.first;
		}
	}

	I::MatSystemSurface->DisableClipping(false);
	I::MatSystemSurface->SetClippingRect(x, y + (nTextH + CFG::Menu_Spacing_Y), x + (w - CFG::Menu_Spacing_X), y + (nTextH + CFG::Menu_Spacing_Y) + h);

	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x + CFG::Menu_Spacing_X, y + (nTextH + CFG::Menu_Spacing_Y) + (h / 2) - 1,
		bActive ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_CENTERY,
		strSelected.empty() ? "None" : strSelected.c_str()
	);

	I::MatSystemSurface->DisableClipping(true);

	if (m_mapStates[&vecSelects])
	{
		bool bSelectRegionHovered = IsHovered(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h * static_cast<int>(vecSelects.size() + 1), &vecSelects);

		if (H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed && !bSelectRegionHovered) {
			m_bClickConsumed = true;
			m_mapStates[&vecSelects] = false;
		}
	}

	if (m_mapStates[&vecSelects])
	{
		H::LateRender->OutlinedRect(x, y + (nTextH + CFG::Menu_Spacing_Y), w, h * static_cast<int>(vecSelects.size() + 1), clr);

		for (int n = 0; n < static_cast<int>(vecSelects.size()); n++)
		{
			const auto &Select = vecSelects[n];

			int nSelectY = (y + (nTextH + CFG::Menu_Spacing_Y) + h) + (h * n);
			bool bSelectHovered = IsHovered(x, nSelectY, w, h, &vecSelects);

			H::LateRender->Rect(x, nSelectY, w, h, clr_dim);

			static int nYesWidth = []() -> int {
				int w = 0, h = 0;
				I::MatSystemSurface->GetTextSize(H::Fonts->Get(EFonts::Menu).m_dwFont, Utils::ConvertUtf8ToWide("Yess").c_str(), w, h);
				return w;
			}();

			H::LateRender->String(
				H::Fonts->Get(EFonts::Menu),
				x + CFG::Menu_Spacing_X, nSelectY,
				bSelectHovered ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
				POS_DEFAULT,
				Select.first, { x, nSelectY, w - nYesWidth, nTextH }
			);

			H::LateRender->String(
				H::Fonts->Get(EFonts::Menu),
				x + w - CFG::Menu_Spacing_X, nSelectY,
				bSelectHovered ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
				POS_LEFT,
				Select.second ? "Yes" : "No", {}
			);

			if (H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed && bSelectHovered) {
				Select.second = !Select.second;
				m_bClickConsumed = true;
			}
		}
	}

	m_nCursorY += h + nTextH + CFG::Menu_Spacing_Y + CFG::Menu_Spacing_Y;

	return bCallback;
}

bool CMenu::ColorPicker(const char *szLabel, Color_t &colVar)
{
	bool bCallback = false;

	int x = m_nCursorX;
	int y = m_nCursorY;
	int w = CFG::Menu_ColorPicker_Preview_Width;
	int h = CFG::Menu_ColorPicker_Preview_Height;

	int w_with_text = [&]() -> int {
		int w_out = 0, h_out = 0;
		I::MatSystemSurface->GetTextSize(H::Fonts->Get(EFonts::Menu).m_dwFont, Utils::ConvertUtf8ToWide(szLabel).c_str(), w_out, h_out);
		return w + w_out + 1;
	}();

	bool bHovered = IsHovered(x, y, w_with_text, h, &colVar);

	if (bHovered && H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed) {
		m_mapStates[&colVar] = !m_mapStates[&colVar];
		m_bClickConsumed = true;
	}

	if (H::Input->IsPressed(VK_ESCAPE) || H::Input->IsPressed(VK_INSERT))
		m_mapStates[&colVar] = false;

	H::Draw->Rect(x, y, w, h, colVar);
	H::Draw->OutlinedRect(x, y, w, h, CFG::Menu_Accent_Primary);
	H::Draw->String(
		H::Fonts->Get(EFonts::Menu),
		x + w + CFG::Menu_Spacing_X,
		y + (h / 2),
		(bHovered || m_mapStates[&colVar]) ? CFG::Menu_Text_Active : CFG::Menu_Text_Inactive,
		POS_CENTERY, szLabel
	);

	if (m_mapStates[&colVar])
	{
		int y = m_nCursorY + h + CFG::Menu_Spacing_Y;
		int w = 200;
		int h = 200;

		bool bHovered = IsHovered(x, y, w, h, &colVar);

		if (H::Input->IsPressed(VK_LBUTTON) && !m_bClickConsumed && !bHovered)
			m_mapStates[&colVar] = false;

		if (H::Input->IsHeld(VK_LBUTTON) && bHovered)
		{
			int x_rel = (H::Input->GetMouseX() - x);
			int y_rel = (H::Input->GetMouseY() - y);

			colVar = *reinterpret_cast<Color_t *>(m_pGradient.get() + (x_rel + y_rel * 200));
		}

		H::LateRender->Texture(m_nColorPickerTextureId, x, y, w, h);
		H::LateRender->OutlinedRect(x, y, w, h, CFG::Menu_Accent_Primary);
	}

	m_nCursorY += h + CFG::Menu_Spacing_Y;

	return bCallback;
}

void CMenu::MainWindow()
{
	Drag(
		CFG::Menu_Pos_X,
		CFG::Menu_Pos_Y,
		CFG::Menu_Width,
		CFG::Menu_Drag_Bar_Height,
		0
	);

	m_bMenuWindowHovered = IsHoveredSimple(
		CFG::Menu_Pos_X,
		CFG::Menu_Pos_Y,
		CFG::Menu_Width,
		CFG::Menu_Height
	);

	H::Draw->Rect(
		CFG::Menu_Pos_X,
		CFG::Menu_Pos_Y,
		CFG::Menu_Width,
		CFG::Menu_Height,
		CFG::Menu_Background
	);

	H::Draw->OutlinedRect(
		CFG::Menu_Pos_X,
		CFG::Menu_Pos_Y,
		CFG::Menu_Width,
		CFG::Menu_Height,
		CFG::Menu_Accent_Primary
	);

	H::Draw->Line(
		CFG::Menu_Pos_X,
		CFG::Menu_Pos_Y + CFG::Menu_Drag_Bar_Height - 1,
		CFG::Menu_Pos_X + CFG::Menu_Width - 1,
		CFG::Menu_Pos_Y + CFG::Menu_Drag_Bar_Height - 1,
		CFG::Menu_Accent_Primary
	);

	m_nCursorX = CFG::Menu_Pos_X + CFG::Menu_Spacing_X;
	m_nCursorY = CFG::Menu_Pos_Y + CFG::Menu_Drag_Bar_Height + CFG::Menu_Spacing_Y;

	enum class EMainTabs { AIM, VISUALS, MISC, PLAYERS, CONFIGS };
	static EMainTabs MainTab = EMainTabs::AIM;

	if (Button("Aim", MainTab == EMainTabs::AIM, CFG::Menu_Tab_Button_Width))
		MainTab = EMainTabs::AIM;

	if (Button("Visuals", MainTab == EMainTabs::VISUALS, CFG::Menu_Tab_Button_Width))
		MainTab = EMainTabs::VISUALS;

	if (Button("Misc", MainTab == EMainTabs::MISC, CFG::Menu_Tab_Button_Width))
		MainTab = EMainTabs::MISC;

	if (Button("Players", MainTab == EMainTabs::PLAYERS, CFG::Menu_Tab_Button_Width))
		MainTab = EMainTabs::PLAYERS;

	if (Button("Configs", MainTab == EMainTabs::CONFIGS, CFG::Menu_Tab_Button_Width))
		MainTab = EMainTabs::CONFIGS;

	H::Draw->Line(
		CFG::Menu_Pos_X + m_nLastButtonW + (CFG::Menu_Spacing_X * 2) - 1,
		CFG::Menu_Pos_Y + CFG::Menu_Drag_Bar_Height,
		CFG::Menu_Pos_X + m_nLastButtonW + (CFG::Menu_Spacing_X * 2) - 1,
		CFG::Menu_Pos_Y + CFG::Menu_Height - 1,
		CFG::Menu_Accent_Primary
	);

	m_nCursorX = CFG::Menu_Pos_X + m_nLastButtonW + (CFG::Menu_Spacing_X * 3) - 1;
	m_nCursorY = CFG::Menu_Pos_Y + CFG::Menu_Drag_Bar_Height + CFG::Menu_Spacing_Y;

	if (MainTab == EMainTabs::AIM)
	{
		enum class EAimTabs { AIMBOT, TRIGGERBOT };
		static EAimTabs AimTab = EAimTabs::AIMBOT;

		int anchor_x = m_nCursorX;
		int anchor_y = m_nCursorY;

		if (Button("Aimbot", AimTab == EAimTabs::AIMBOT))
			AimTab = EAimTabs::AIMBOT;

		m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
		m_nCursorY = anchor_y;

		if (Button("Triggerbot", AimTab == EAimTabs::TRIGGERBOT))
			AimTab = EAimTabs::TRIGGERBOT;

		H::Draw->Line(
			anchor_x - CFG::Menu_Spacing_X,
			m_nCursorY,
			CFG::Menu_Pos_X + CFG::Menu_Width - 1,
			m_nCursorY,
			CFG::Menu_Accent_Primary
		);

		m_nCursorX = anchor_x + CFG::Menu_Spacing_X;
		m_nCursorY += CFG::Menu_Spacing_Y;

		if (AimTab == EAimTabs::AIMBOT)
		{
			anchor_y = m_nCursorY;

			GroupBoxStart("Global", 150);
			{
				CheckBox("Active", CFG::Aimbot_Active);
				CheckBox("Auto Shoot", CFG::Aimbot_AutoShoot);
				InputKey("Key", CFG::Aimbot_Key);

				multiselect("Targets", AimbotTargets, {
					{ "Players", CFG::Aimbot_Target_Players },
					{ "Buildings", CFG::Aimbot_Target_Buildings }
					});

				multiselect("Ignore", AimbotIgnores, {
					{ "Friends", CFG::Aimbot_Ignore_Friends },
					{ "Invisible", CFG::Aimbot_Ignore_Invisible },
					{ "Invulnerable", CFG::Aimbot_Ignore_Invulnerable },
					{ "Taunting", CFG::Aimbot_Ignore_Taunting }
					});
			}
			GroupBoxEnd();

			GroupBoxStart("Melee", 150);
			{
				CheckBox("Active", CFG::Aimbot_Melee_Active);
				CheckBox("Always Active", CFG::Aimbot_Melee_Always_Active);
				CheckBox("Target Lag Records", CFG::Aimbot_Melee_Target_LagRecords);

				CheckBox("Predict Swing", CFG::Aimbot_Melee_Predict_Swing);
				CheckBox("Walk To Target", CFG::Aimbot_Melee_Walk_To_Target);
				CheckBox("Whip Teammates", CFG::Aimbot_Melee_Whip_Teammates);

				SelectSingle("Aim Type", CFG::Aimbot_Melee_Aim_Type, {
					{ "Normal", 0 },
					{ "Silent", 1 },
					{ "Smooth", 2 }
					});

				SelectSingle("Sort", CFG::Aimbot_Melee_Sort, {
					{ "FOV", 0 },
					{ "Distance", 1 }
					});

				SliderFloat("FOV", CFG::Aimbot_Melee_FOV, 1.0f, 180.0f, 1.0f, "%.0f");
				SliderFloat("Smoothing", CFG::Aimbot_Melee_Smoothing, 0.0f, 20.0f, 0.5f, "%.1f");
				SliderFloat("Predict Swing Time", CFG::Aimbot_Melee_Predict_Swing_Amount, 0.1f, 0.2f, 0.01f, "%.2f");
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Hitscan", 150);
			{
				CheckBox("Active", CFG::Aimbot_Hitscan_Active);
				CheckBox("Target Lag Records", CFG::Aimbot_Hitscan_Target_LagRecords);
				CheckBox("Target Stickies", CFG::Aimbot_Hitscan_Target_Stickies);

				CheckBox("Smooth Auto Shoot", CFG::Aimbot_Hitscan_Advanced_Smooth_AutoShoot);
				CheckBox("Auto Scope", CFG::Aimbot_Hitscan_Auto_Scope);
				CheckBox("Wait For Headshot", CFG::Aimbot_Hitscan_Wait_For_Headshot);
				CheckBox("Wait For Charge", CFG::Aimbot_Hitscan_Wait_For_Charge);
				CheckBox("Minigun Tapfire", CFG::Aimbot_Hitscan_Minigun_TapFire);

				SelectSingle("Aim Type", CFG::Aimbot_Hitscan_Aim_Type, {
					{ "Normal", 0 },
					{ "Silent", 1 },
					{ "Smooth", 2 }
				});

				SelectSingle("Hitbox", CFG::Aimbot_Hitscan_Hitbox, {
					{ "Head", 0 },
					{ "Body", 1 },
					{ "Auto", 2 }
					});

				SelectSingle("Sort", CFG::Aimbot_Hitscan_Sort, {
					{ "FOV", 0 },
					{ "Distance", 1 }
					});

				multiselect("Scan", HitscanScan, {
					{ "Head", CFG::Aimbot_Hitscan_Scan_Head },
					{ "Body", CFG::Aimbot_Hitscan_Scan_Body },
					{ "Arms", CFG::Aimbot_Hitscan_Scan_Arms },
					{ "Legs", CFG::Aimbot_Hitscan_Scan_Legs },
					{ "Buildings", CFG::Aimbot_Hitscan_Scan_Buildings }
					});

				SliderFloat("FOV", CFG::Aimbot_Hitscan_FOV, 1.0f, 180.0f, 1.0f, "%.0f");
				SliderFloat("Smoothing", CFG::Aimbot_Hitscan_Smoothing, 0.0f, 20.0f, 0.5f, "%.1f");
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Projectile", 150);
			{
				CheckBox("Active", CFG::Aimbot_Projectile_Active);
				CheckBox("No Spread", CFG::Aimbot_Projectile_NoSpread);
				CheckBox("Auto Double Donk", CFG::Aimbot_Projectile_Auto_Double_Donk);
				CheckBox("Advanced Head Aim", CFG::Aimbot_Projectile_Advanced_Head_Aim);
				CheckBox("Ground Strafe Prediction", CFG::Aimbot_Projectile_Ground_Strafe_Prediction);
				CheckBox("Air Strafe Prediction", CFG::Aimbot_Projectile_Air_Strafe_Prediction);
				CheckBox("BBOX Multipoint", CFG::Aimbot_Projectile_BBOX_Multipoint);
				SelectSingle("Rocket Splash", CFG::Aimbot_Projectile_Rocket_Splash,
				{
					{ "Disabled", 0 },
					{ "Enabled", 1 },
					{ "Preferred", 2 }
				});

				SelectSingle("Aim Type", CFG::Aimbot_Projectile_Aim_Type, {
					{ "Normal", 0 },
					{ "Silent", 1 }
					});

				SelectSingle("Aim Position", CFG::Aimbot_Projectile_Aim_Position, {
					{ "Feet", 0 },
					{ "Body", 1 },
					{ "Head", 2 },
					{ "Auto", 3 }
					});

				SelectSingle("Sort", CFG::Aimbot_Projectile_Sort, {
					{ "FOV", 0 },
					{ "Distance", 1 }
					});

				SelectSingle("Prediction Method", CFG::Aimbot_Projectile_Aim_Prediction_Method, {
					{ "Full Acceleration", 0 },
					{ "Current Velocity", 1 }
				});

				SliderFloat("FOV", CFG::Aimbot_Projectile_FOV, 1.0f, 180.0f, 1.0f, "%.0f");
				SliderFloat("Max Simulation Time", CFG::Aimbot_Projectile_Max_Simulation_Time, 1.0f, 5.0f, 0.5f, "%.1fs");
				SliderInt("Max Targets", CFG::Aimbot_Projectile_Max_Processing_Targets, 1, 6, 1);
			}
			GroupBoxEnd();
		}

		if (AimTab == EAimTabs::TRIGGERBOT)
		{
			anchor_y = m_nCursorY;

			GroupBoxStart("Global", 150);
			{
				CheckBox("Active", CFG::Triggerbot_Active);
				InputKey("Key", CFG::Triggerbot_Key);
			}
			GroupBoxEnd();

			GroupBoxStart("Auto Airblast", 150);
			{
				CheckBox("Active", CFG::Triggerbot_AutoAirblast_Active);
				CheckBox("Aim Assist", CFG::Triggerbot_AutoAirblast_Aim_Assist);

				SelectSingle("Mode", CFG::Triggerbot_AutoAirblast_Mode,
				{
					{ "Legit", 0 },
					{ "Rage", 1 }
				});

				SelectSingle("Aim Mode", CFG::Triggerbot_AutoAirblast_Aim_Mode,
				{
					{ "Normal", 0 },
					{ "Silent", 1 }
				});

				multiselect("Ignore", TriggerbotAirblastIgnore,
				{
					{ "Rocket", CFG::Triggerbot_AutoAirblast_Ignore_Rocket },
					{ "Sentry Rocket", CFG::Triggerbot_AutoAirblast_Ignore_SentryRocket },
					{ "Jarate", CFG::Triggerbot_AutoAirblast_Ignore_Jar },
					{ "Gas", CFG::Triggerbot_AutoAirblast_Ignore_JarGas },
					{ "Milk", CFG::Triggerbot_AutoAirblast_Ignore_JarMilk },
					{ "Arrow", CFG::Triggerbot_AutoAirblast_Ignore_Arrow },
					{ "Flare", CFG::Triggerbot_AutoAirblast_Ignore_Flare },
					{ "Cleaver", CFG::Triggerbot_AutoAirblast_Ignore_Cleaver },
					{ "Healing Bolt", CFG::Triggerbot_AutoAirblast_Ignore_HealingBolt },
					{ "Pipebomb", CFG::Triggerbot_AutoAirblast_Ignore_PipebombProjectile },
					{ "Ball of Fire", CFG::Triggerbot_AutoAirblast_Ignore_BallOfFire },
					{ "Energy Ring", CFG::Triggerbot_AutoAirblast_Ignore_EnergyRing },
					{ "Energy Ball", CFG::Triggerbot_AutoAirblast_Ignore_EnergyBall },
				});
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Auto Detonate", 150);
			{
				CheckBox("Active", CFG::Triggerbot_AutoDetonate_Active);

				multiselect("Targets", DetonateTargets, {
					{ "Players", CFG::Triggerbot_AutoDetonate_Target_Players },
					{ "Buildings", CFG::Triggerbot_AutoDetonate_Target_Buildings }
					});

				multiselect("Ignore", DetonateIgnores, {
					{ "Friends", CFG::Triggerbot_AutoDetonate_Ignore_Friends },
					{ "Invisible", CFG::Triggerbot_AutoDetonate_Ignore_Invisible },
					{ "Invulnerable", CFG::Triggerbot_AutoDetonate_Ignore_Invulnerable }
					});
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Auto Backstab", 150);
			{
				CheckBox("Active", CFG::Triggerbot_AutoBackstab_Active);
				CheckBox("Knife If Lethal", CFG::Triggerbot_AutoBackstab_Knife_If_Lethal);

				SelectSingle("Mode", CFG::Triggerbot_AutoBacktab_Mode,
				{
					{ "Legit", 0 },
					{ "Rage", 1 }
				});

				SelectSingle("Aim Mode", CFG::Triggerbot_AutoBacktab_Aim_Mode,
				{
					{ "Normal", 0 },
					{ "Silent", 1 }
				});

				multiselect("Ignore", AutoBackstabIgnores,
				{
					{ "Friends", CFG::Triggerbot_AutoBackstab_Ignore_Friends },
					{ "Invisible", CFG::Triggerbot_AutoBackstab_Ignore_Invisible },
					{ "Invulnerable", CFG::Triggerbot_AutoBackstab_Ignore_Invulnerable }
				});
			}
			GroupBoxEnd();
		}
	}

	if (MainTab == EMainTabs::VISUALS)
	{
		enum class EVisualsTabs { ESP, RADAR, MATERIALS, OUTLINES, OTHER, OTHER2, COLORS };
		static EVisualsTabs VisualsTab = EVisualsTabs::ESP;

		int anchor_x = m_nCursorX;
		int anchor_y = m_nCursorY;

		if (Button("ESP", VisualsTab == EVisualsTabs::ESP))
			VisualsTab = EVisualsTabs::ESP;

		m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
		m_nCursorY = anchor_y;

		if (Button("Radar", VisualsTab == EVisualsTabs::RADAR))
			VisualsTab = EVisualsTabs::RADAR;

		m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
		m_nCursorY = anchor_y;

		if (Button("Materials", VisualsTab == EVisualsTabs::MATERIALS))
			VisualsTab = EVisualsTabs::MATERIALS;

		m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
		m_nCursorY = anchor_y;

		if (Button("Outlines", VisualsTab == EVisualsTabs::OUTLINES))
			VisualsTab = EVisualsTabs::OUTLINES;

		m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
		m_nCursorY = anchor_y;

		if (Button("Other", VisualsTab == EVisualsTabs::OTHER))
			VisualsTab = EVisualsTabs::OTHER;

		m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
		m_nCursorY = anchor_y;

		if (Button("Other2", VisualsTab == EVisualsTabs::OTHER2))
			VisualsTab = EVisualsTabs::OTHER2;

		m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
		m_nCursorY = anchor_y;

		if (Button("Colors", VisualsTab == EVisualsTabs::COLORS))
			VisualsTab = EVisualsTabs::COLORS;

		H::Draw->Line(
			anchor_x - CFG::Menu_Spacing_X,
			m_nCursorY,
			CFG::Menu_Pos_X + CFG::Menu_Width - 1,
			m_nCursorY,
			CFG::Menu_Accent_Primary
		);

		m_nCursorX = anchor_x + CFG::Menu_Spacing_X;
		m_nCursorY += CFG::Menu_Spacing_Y;

		if (VisualsTab == EVisualsTabs::ESP)
		{
			anchor_y = m_nCursorY;

			GroupBoxStart("Global", 150);
			{
				CheckBox("Active", CFG::ESP_Active);
				SelectSingle("Tracer From", CFG::ESP_Tracer_From, { { "Top", 0 }, { "Center", 1 }, { "Bottom", 2 } });
				SelectSingle("Tracer To", CFG::ESP_Tracer_To, { { "Top", 0 }, { "Center", 1 }, { "Bottom", 2 } });
				SelectSingle("Text Color", CFG::ESP_Text_Color, { { "Default", 0 }, { "White", 1 } });
			}
			GroupBoxEnd();

			GroupBoxStart("World", 150);
			{
				CheckBox("Active", CFG::ESP_World_Active);
				SliderFloat("Alpha", CFG::ESP_World_Alpha, 0.1f, 1.0f, 0.1f, "%.1f");

				multiselect("Ignore", WorldIgnore, {
					{ "Health Packs", CFG::ESP_World_Ignore_HealthPacks },
					{ "Ammo Packs", CFG::ESP_World_Ignore_AmmoPacks },
					{ "Local Projectiles", CFG::ESP_World_Ignore_LocalProjectiles },
					{ "Enemy Projectiles", CFG::ESP_World_Ignore_EnemyProjectiles },
					{ "Teammate Projectiles", CFG::ESP_World_Ignore_TeammateProjectiles },
					{ "Halloween Gifts", CFG::ESP_World_Ignore_Halloween_Gift },
					{ "MVM Money", CFG::ESP_World_Ignore_MVM_Money }
				});

				multiselect("Draw", WorldDraw, {
					{ "Name", CFG::ESP_World_Name },
					{ "Box", CFG::ESP_World_Box },
					{ "Tracer", CFG::ESP_World_Tracer }
				});
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Players", 150);
			{
				CheckBox("Active", CFG::ESP_Players_Active);
				SliderFloat("Alpha", CFG::ESP_Players_Alpha, 0.1f, 1.0f, 0.1f, "%.1f");
				SliderFloat("Arrow Radius", CFG::ESP_Players_Arrows_Radius, 50.0f, 400.0f, 50.0f, "%.0f");
				SliderFloat("Arrow Max Distance", CFG::ESP_Players_Arrows_Max_Distance, 100.0f, 1000.0f, 100.0f, "%.0f");
				SelectSingle("Bones Color", CFG::ESP_Players_Bones_Color, { { "Default", 0 }, { "White", 1 } });

				multiselect("Ignore", PlayerIgnore, {
					{ "Local", CFG::ESP_Players_Ignore_Local },
					{ "Friends", CFG::ESP_Players_Ignore_Friends },
					{ "Enemies", CFG::ESP_Players_Ignore_Enemies },
					{ "Teammates", CFG::ESP_Players_Ignore_Teammates },
					{ "Invisible", CFG::ESP_Players_Ignore_Invisible }
					});

				multiselect("Draw", PlayerDraw, {
					{ "Name", CFG::ESP_Players_Name },
					{ "Class", CFG::ESP_Players_Class },
					{ "Class Icon", CFG::ESP_Players_Class_Icon },
					{ "Health", CFG::ESP_Players_Health },
					{ "Health Bar", CFG::ESP_Players_HealthBar },
					{ "Uber", CFG::ESP_Players_Uber },
					{ "Uber Bar", CFG::ESP_Players_UberBar },
					{ "Box", CFG::ESP_Players_Box },
					{ "Tracer", CFG::ESP_Players_Tracer },
					{ "Bones", CFG::ESP_Players_Bones },
					{ "Arrows", CFG::ESP_Players_Arrows },
					{ "Conds", CFG::ESP_Players_Conds },
					{ "Sniper Lines", CFG::ESP_Players_Sniper_Lines }
					});

				CheckBox("Show Team Medics", CFG::ESP_Players_Show_Teammate_Medics);
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Buildings", 150);
			{
				CheckBox("Active", CFG::ESP_Buildings_Active);
				SliderFloat("Alpha", CFG::ESP_Buildings_Alpha, 0.1f, 1.0f, 0.1f, "%.1f");

				multiselect("Ignore", BuildingIgnore, {
					{ "Local", CFG::ESP_Buildings_Ignore_Local },
					{ "Enemies", CFG::ESP_Buildings_Ignore_Enemies },
					{ "Teammates", CFG::ESP_Buildings_Ignore_Teammates }
					});

				multiselect("Draw", BuildingDraw, {
					{ "Name", CFG::ESP_Buildings_Name },
					{ "Health", CFG::ESP_Buildings_Health },
					{ "Health Bar", CFG::ESP_Buildings_HealthBar },
					{ "Level", CFG::ESP_Buildings_Level },
					{ "Level Bar", CFG::ESP_Buildings_LevelBar },
					{ "Box", CFG::ESP_Buildings_Box },
					{ "Tracer", CFG::ESP_Buildings_Tracer },
					{ "Conds", CFG::ESP_Buildings_Conds }
					});

				CheckBox("Show Team Dispensers", CFG::ESP_Buildings_Show_Teammate_Dispensers);
			}
			GroupBoxEnd();
		}

		if (VisualsTab == EVisualsTabs::RADAR)
		{
			anchor_x = m_nCursorX;
			anchor_y = m_nCursorY;

			GroupBoxStart("Global", 150);
			{
				CheckBox("Active", CFG::Radar_Active);
				SelectSingle("Style", CFG::Radar_Style, { { "Rectangle", 0 }, { "Circle", 1 } });
				SliderInt("Size", CFG::Radar_Size, 100, 1000, 25);
				SliderInt("Icon Size", CFG::Radar_Icon_Size, 18, 36, 2);
				SliderFloat("Radius", CFG::Radar_Radius, 100.0f, 3000.0f, 50.0f, "%.0f");
				SliderFloat("Cross Alpha", CFG::Radar_Cross_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");
				SliderFloat("Outline Alpha", CFG::Radar_Outline_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");
				SliderFloat("Background Alpha", CFG::Radar_Background_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Players", 150);
			{
				CheckBox("Active", CFG::Radar_Players_Active);

				multiselect("Ignore", PlayerIgnore, {
					{ "Local", CFG::Radar_Players_Ignore_Local },
					{ "Friends", CFG::Radar_Players_Ignore_Friends },
					{ "Enemies", CFG::Radar_Players_Ignore_Enemies },
					{ "Teammates", CFG::Radar_Players_Ignore_Teammates },
					{ "Invisible", CFG::Radar_Players_Ignore_Invisible }
					});

				CheckBox("Show Team Medics", CFG::Radar_Players_Show_Teammate_Medics);
			}
			GroupBoxEnd();

			GroupBoxStart("Buildings", 150);
			{
				CheckBox("Active", CFG::Radar_Buildings_Active);

				multiselect("Ignore", BuildingIgnore, {
					{ "Local", CFG::Radar_Buildings_Ignore_Local },
					{ "Enemies", CFG::Radar_Buildings_Ignore_Enemies },
					{ "Teammates", CFG::Radar_Buildings_Ignore_Teammates }
					});

				CheckBox("Show Team Dispensers", CFG::Radar_Buildings_Show_Teammate_Dispensers);
			}
			GroupBoxEnd();

			GroupBoxStart("World", 150);
			{
				CheckBox("Active", CFG::Radar_World_Active);

				multiselect("Ignore", BuildingIgnore, {
					{ "Health Packs", CFG::Radar_World_Ignore_HealthPacks },
					{ "Ammo Packs", CFG::Radar_World_Ignore_AmmoPacks },
					{ "Halloween Gifts", CFG::Radar_World_Ignore_Halloween_Gift },
					{ "MVM Money", CFG::Radar_World_Ignore_MVM_Money }
				});
			}
			GroupBoxEnd();
		}

		if (VisualsTab == EVisualsTabs::MATERIALS)
		{
			anchor_x = m_nCursorX;
			anchor_y = m_nCursorY;

			GroupBoxStart("Global", 150);
			{
				CheckBox("Active", CFG::Materials_Active);
			}
			GroupBoxEnd();

			GroupBoxStart("World", 150);
			{
				CheckBox("Active", CFG::Materials_World_Active);
				CheckBox("No Depth", CFG::Materials_World_No_Depth);
				SliderFloat("Alpha", CFG::Materials_World_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");

				SelectSingle("Material", CFG::Materials_World_Material, {
					{ "Original", 0 },
					{ "Flat", 1 },
					{ "Shaded", 2 },
					{ "Glossy", 3 },
					{ "Glow", 4 },
					{ "Plastic", 5 }
					});

				multiselect("Ignore", WorldIgnore, {
					{ "Health Packs", CFG::Materials_World_Ignore_HealthPacks },
					{ "Ammo Packs", CFG::Materials_World_Ignore_AmmoPacks },
					{ "Local Projectiles", CFG::Materials_World_Ignore_LocalProjectiles },
					{ "Enemy Projectiles", CFG::Materials_World_Ignore_EnemyProjectiles },
					{ "Teammate Projectiles", CFG::Materials_World_Ignore_TeammateProjectiles },
					{ "Halloween Gifts", CFG::Materials_World_Ignore_Halloween_Gift },
					{ "MVM Money", CFG::Materials_World_Ignore_MVM_Money }
					});
			}
			GroupBoxEnd();

			GroupBoxStart("View Model", 150);
			{
				CheckBox("Active", CFG::Materials_ViewModel_Active);

				SliderFloat("Hands Alpha", CFG::Materials_ViewModel_Hands_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");

				SelectSingle("Hands Material", CFG::Materials_ViewModel_Hands_Material, {
					{ "Original", 0 },
					{ "Flat", 1 },
					{ "Shaded", 2 },
					{ "Glossy", 3 },
					{ "Glow", 4 },
					{ "Plastic", 5 }
				});

				SliderFloat("Weapon Alpha", CFG::Materials_ViewModel_Weapon_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");

				SelectSingle("Weapon Material", CFG::Materials_ViewModel_Weapon_Material, {
					{ "Original", 0 },
					{ "Flat", 1 },
					{ "Shaded", 2 },
					{ "Glossy", 3 },
					{ "Glow", 4 },
					{ "Plastic", 5 }
				});
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Players", 150);
			{
				CheckBox("Active", CFG::Materials_Players_Active);
				CheckBox("No Depth", CFG::Materials_Players_No_Depth);
				SliderFloat("Alpha", CFG::Materials_Players_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");

				SelectSingle("Material", CFG::Materials_Players_Material, {
					{ "Original", 0 },
					{ "Flat", 1 },
					{ "Shaded", 2 },
					{ "Glossy", 3 },
					{ "Glow", 4 },
					{ "Plastic", 5 }
					});

				SelectSingle("Lag Records Style", CFG::Materials_Players_LagRecords_Style, {
					{ "All", 0 },
					{ "Last Only", 1 }
					});

				multiselect("Ignore", PlayerIgnore, {
					{ "Local", CFG::Materials_Players_Ignore_Local },
					{ "Friends", CFG::Materials_Players_Ignore_Friends },
					{ "Enemies", CFG::Materials_Players_Ignore_Enemies },
					{ "Teammates", CFG::Materials_Players_Ignore_Teammates },
					{ "Lag Records", CFG::Materials_Players_Ignore_LagRecords }
					});

				CheckBox("Show Team Medics", CFG::Materials_Players_Show_Teammate_Medics);
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Buildings", 150);
			{
				CheckBox("Active", CFG::Materials_Buildings_Active);
				CheckBox("No Depth", CFG::Materials_Buildings_No_Depth);
				SliderFloat("Alpha", CFG::Materials_Buildings_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");

				SelectSingle("Material", CFG::Materials_Buildings_Material, {
					{ "Original", 0 },
					{ "Flat", 1 },
					{ "Shaded", 2 },
					{ "Glossy", 3 },
					{ "Glow", 4 },
					{ "Plastic", 5 }
					});

				multiselect("Ignore", BuildingIgnore, {
					{ "Local", CFG::Materials_Buildings_Ignore_Local },
					{ "Enemies", CFG::Materials_Buildings_Ignore_Enemies },
					{ "Teammates", CFG::Materials_Buildings_Ignore_Teammates }
					});

				CheckBox("Show Team Dispensers", CFG::Materials_Buildings_Show_Teammate_Dispensers);
			}
			GroupBoxEnd();
		}

		if (VisualsTab == EVisualsTabs::OUTLINES)
		{
			anchor_x = m_nCursorX;
			anchor_y = m_nCursorY;

			GroupBoxStart("Global", 150);
			{
				CheckBox("Active", CFG::Outlines_Active);

				SelectSingle("Style", CFG::Outlines_Style, {
					{ "Bloom", 0 },
					{ "Crisp", 1 },
					{ "Cartoony", 2 },
					{ "Cartoony Alt", 3 }
				});

				SliderInt("Bloom Amount", CFG::Outlines_Bloom_Amount, 1, 10, 1);
			}
			GroupBoxEnd();

			GroupBoxStart("World", 150);
			{
				CheckBox("Active", CFG::Outlines_World_Active);
				SliderFloat("Alpha", CFG::Outlines_World_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");

				multiselect("Ignore", WorldIgnore, {
					{ "Health Packs", CFG::Outlines_World_Ignore_HealthPacks },
					{ "Ammo Packs", CFG::Outlines_World_Ignore_AmmoPacks },
					{ "Local Projectiles", CFG::Outlines_World_Ignore_LocalProjectiles },
					{ "Enemy Projectiles", CFG::Outlines_World_Ignore_EnemyProjectiles },
					{ "Teammate Projectiles", CFG::Outlines_World_Ignore_TeammateProjectiles },
					{ "Halloween Gifts", CFG::Outlines_World_Ignore_Halloween_Gift },
					{ "MVM Money", CFG::Outlines_World_Ignore_MVM_Money }
					});
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Players", 150);
			{
				CheckBox("Active", CFG::Outlines_Players_Active);
				SliderFloat("Alpha", CFG::Outlines_Players_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");

				multiselect("Ignore", PlayerIgnore, {
					{ "Local", CFG::Outlines_Players_Ignore_Local },
					{ "Friends", CFG::Outlines_Players_Ignore_Friends },
					{ "Enemies", CFG::Outlines_Players_Ignore_Enemies },
					{ "Teammates", CFG::Outlines_Players_Ignore_Teammates }
					});

				CheckBox("Show Team Medics", CFG::Outlines_Players_Show_Teammate_Medics);
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Buildings", 150);
			{
				CheckBox("Active", CFG::Outlines_Buildings_Active);
				SliderFloat("Alpha", CFG::Outlines_Buildings_Alpha, 0.0f, 1.0f, 0.1f, "%.1f");

				multiselect("Ignore", BuildingIgnore, {
					{ "Local", CFG::Outlines_Buildings_Ignore_Local },
					{ "Enemies", CFG::Outlines_Buildings_Ignore_Enemies },
					{ "Teammates", CFG::Outlines_Buildings_Ignore_Teammates }
					});

				CheckBox("Show Team Dispensers", CFG::Outlines_Buildings_Show_Teammate_Dispensers);
			}
			GroupBoxEnd();
		}

		if (VisualsTab == EVisualsTabs::OTHER)
		{
			anchor_x = m_nCursorX;
			anchor_y = m_nCursorY;

			GroupBoxStart("Local", 150);
			{
				CheckBox("Aimbot FOV Circle", CFG::Visuals_Aimbot_FOV_Circle);
				SliderFloat("FOV Circle Alpha", CFG::Visuals_Aimbot_FOV_Circle_Alpha, 0.01f, 1.0f, 0.01f, "%.2f");
				CheckBox("Draw Projectile Arc", CFG::Visuals_Draw_Projectile_Arc);
				CheckBox("Reveal Scoreboard", CFG::Visuals_Reveal_Scoreboard);
				CheckBox("Clean Screenshot", CFG::Misc_Clean_Screenshot);
				SliderFloat("FOV Override", CFG::Visuals_FOV_Override, 70.0f, 170.0f, 1.0f, "%.0f");

				multiselect("Removals", LocalRemovals, {
					{ "Scope", CFG::Visuals_Remove_Scope },
					{ "Zoom", CFG::Visuals_Remove_Zoom },
					{ "Punch", CFG::Visuals_Remove_Punch },
					{ "Screen Overlay", CFG::Visuals_Remove_Screen_Overlay },
					{ "Screen Shake", CFG::Visuals_Remove_Screen_Shake },
					{ "Screen Fade", CFG::Visuals_Remove_Screen_Fade }
				});

				SelectSingle("Removals Mode", CFG::Visuals_Removals_Mode, {
					{ "Everyone", 0 },
					{ "Local Only", 1 }
					});

				SelectSingle("Tracer Effect", CFG::Visuals_Tracer_Type, {
					{ "Default", 0 },
					{ "C.A.P.P.E.R", 1 },
					{ "Machina (White)", 2 },
					{ "Machina (Team)", 3 },
					{ "Big Nasty", 4 },
					{ "Short Circuit", 5 },
					{ "Mrasmus Zap", 6 },
					{ "Random", 7 },
					{ "Random (No Zap)", 8 }
					});

				SelectSingle("Projectile Arc Color Mode", CFG::Visuals_Draw_Projectile_Arc_Color_Mode,
				{
					{ "Custom", 0 },
					{ "Rainbow", 1 }
				});

				SelectSingle("Movement Path Style", CFG::Visuals_Draw_Movement_Path_Style,
				{
					{ "Disabled", 0 },
					{ "Line", 1 },
					{ "Dashed Line", 2 },
					{ "Alt Line", 3 }
				});
			}
			GroupBoxEnd();

			GroupBoxStart("Chat", 150);
			{
				CheckBox("Teammate Votes", CFG::Visuals_Chat_Teammate_Votes);
				CheckBox("Enemy Votes", CFG::Visuals_Chat_Enemy_Votes);
				CheckBox("Player List Info", CFG::Visuals_Chat_Player_List_Info);
				CheckBox("Name Tags", CFG::Visuals_Chat_Name_Tags);
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("World", 150);
			{
				CheckBox("Flat Textures", CFG::Visuals_Flat_Textures);
				CheckBox("Disable Fog", CFG::Visuals_Remove_Fog);
				CheckBox("Disable Sky Fog", CFG::Visuals_Remove_Sky_Fog);
				CheckBox("Distance Prop Alpha", CFG::Visuals_Distance_Prop_Alpha);
				CheckBox("Don't Modulate Sky", CFG::Visuals_World_Modulation_No_Sky_Change);

				SelectSingle("World Modulation Mode", CFG::Visuals_World_Modulation_Mode,
				{
					{ "Night Mode", 0 },
					{ "Custom Color", 1 }
				});

				SliderFloat("Night Mode", CFG::Visuals_Night_Mode, 0.0f, 100.0f, 1.0f, "%.0f");

				SelectSingle("Particles Mode", CFG::Visuals_Particles_Mode, {
					{ "Original", 0 },
					{ "Custom Color", 1 },
					{ "Rainbow", 2 }
				});

				SliderFloat("Particles Rainbow Rate", CFG::Visuals_Particles_Rainbow_Rate, 1.0f, 10.0f, 1.0f, "%.0f");
			}
			GroupBoxEnd();

			GroupBoxStart("Spectator List", 150);
			{
				CheckBox("Active", CFG::Visuals_SpectatorList_Active);
				SliderFloat("Outline Alpha", CFG::Visuals_SpectatorList_Outline_Alpha, 0.1f, 1.0f, 0.1f, "%.1f");
				SliderFloat("Background Alpha", CFG::Visuals_SpectatorList_Background_Alpha, 0.1f, 1.0f, 0.1f, "%.1f");
				SliderInt("Width", CFG::Visuals_SpectatorList_Width, 200, 1000, 1);
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Thirdperson", 150);
			{
				CheckBox("Active", CFG::Visuals_Thirdperson_Active);
				InputKey("Toggle Key", CFG::Visuals_Thirdperson_Key);
				SliderFloat("Offset Forward", CFG::Visuals_Thirdperson_Offset_Forward, 10.0f, 200.0f, 1.0f, "%.0f");
				SliderFloat("Offset Right", CFG::Visuals_Thirdperson_Offset_Right, -50.0f, 50.0f, 1.0f, "%.0f");
				SliderFloat("Offset Up", CFG::Visuals_Thirdperson_Offset_Up, -50.0f, 50.0f, 1.0f, "%.0f");
			}
			GroupBoxEnd();

			GroupBoxStart("View Model", 150);
			{
				CheckBox("Active", CFG::Visuals_ViewModel_Active);
				CheckBox("Sway", CFG::Visuals_ViewModel_Sway);
				SliderFloat("Sway Scale", CFG::Visuals_ViewModel_Sway_Scale, 0.1f, 1.0f, 0.1f, "%.1f");
				SliderFloat("Offset Forward", CFG::Visuals_ViewModel_Offset_Forward, -50.00f, 50.0f, 1.0f, "%.0f");
				SliderFloat("Offset Right", CFG::Visuals_ViewModel_Offset_Right, -50.0f, 50.0f, 1.0f, "%.0f");
				SliderFloat("Offset Up", CFG::Visuals_ViewModel_Offset_Up, -50.0f, 50.0f, 1.0f, "%.0f");
			}
			GroupBoxEnd();
		}

		if (VisualsTab == EVisualsTabs::OTHER2)
		{
			anchor_x = m_nCursorX;
			anchor_y = m_nCursorY;

			GroupBoxStart("Performance", 150);
			{
				CheckBox("Disable Detail Props", CFG::Visuals_Disable_Detail_Props);
				CheckBox("Disable Ragdolls", CFG::Visuals_Disable_Ragdolls);
				CheckBox("Disable Wearables", CFG::Visuals_Disable_Wearables);
				CheckBox("Disable Post Processing", CFG::Visuals_Disable_Post_Processing);
				CheckBox("Disable Dropped Weapons", CFG::Visuals_Disable_Dropped_Weapons);
				CheckBox("Use Simple Models", CFG::Visuals_Simple_Models);
			}
			GroupBoxEnd();

			GroupBoxStart("Paint", 150);
			{
				CheckBox("Active", CFG::Visuals_Paint_Active);
				InputKey("Key", CFG::Visuals_Paint_Key);
				InputKey("Erase Key", CFG::Visuals_Paint_Erase_Key);
				const char *pszFmt = CFG::Visuals_Paint_LifeTime <= 0.0f ? "inf" : "%.0fs";
				SliderFloat("Life Time", CFG::Visuals_Paint_LifeTime, 0.0f, 10.0f, 1.0f, pszFmt);
				SliderInt("Bloom Amount", CFG::Visuals_Paint_Bloom_Amount, 3, 10, 1);
			}
			GroupBoxEnd();

			GroupBoxStart("Team Well-Being", 150);
			{
				CheckBox("Active", CFG::Visuals_TeamWellBeing_Active);
				CheckBox("Medic Only", CFG::Visuals_TeamWellBeing_Medic_Only);
				SliderFloat("Background Alpha", CFG::Visuals_TeamWellBeing_Background_Alpha, 0.1f, 1.0f, 0.1f, "%.1f");
				SliderInt("Width", CFG::Visuals_TeamWellBeing_Width, 200, 1000, 1);
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Spy Camera", 150);
			{
				CheckBox("Active", CFG::Visuals_SpyCamera_Active);
				SliderFloat("Background Alpha", CFG::Visuals_SpyCamera_Background_Alpha, 0.1f, 1.0f, 0.1f, "%.1f");
				SliderInt("Camera Width", CFG::Visuals_SpyCamera_Pos_W, 100, 600, 10);
				SliderInt("Camera Height", CFG::Visuals_SpyCamera_Pos_H, 100, 600, 10);
				SliderFloat("Camera FOV", CFG::Visuals_SpyCamera_FOV, 70.0f, 170.0f, 1.0f, "%.0f");
			}
			GroupBoxEnd();

			GroupBoxStart("Spy Warning", 150);
			{
				CheckBox("Active", CFG::Viuals_SpyWarning_Active);
				CheckBox("Announce", CFG::Viuals_SpyWarning_Announce);

				multiselect("Ignore", SpyWarningIgnore, {
					{ "Cloaked", CFG::Viuals_SpyWarning_Ignore_Cloaked },
					{ "Friends", CFG::Viuals_SpyWarning_Ignore_Friends },
					{ "Invisible", CFG::Viuals_SpyWarning_Ignore_Invisible }
				});
			}
			GroupBoxEnd();

			GroupBoxStart("Ragdolls", 150);
			{
				CheckBox("Active", CFG::Visuals_Ragdolls_Active);
				CheckBox("No Gib", CFG::Visuals_Ragdolls_No_Gib);
				CheckBox("No Death Animation", CFG::Visuals_Ragdolls_No_Death_Anim);

				SelectSingle("Effect", CFG::Visuals_Ragdolls_Effect, {
					{ "Default", 0 },
					{ "Burning", 1 },
					{ "Electrocuted", 2 },
					{ "Ash", 3 },
					{ "Gold", 4 },
					{ "Ice", 5 },
					{ "Dissolve", 6 },
					{ "Random", 7 }
					});

				SliderFloat("Force Multiplier", CFG::Visuals_Ragdolls_Force_Mult, 0.0f, 5.0f, 1.0f, "%.0f");
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Beams", 150);
			{
				CheckBox("Active", CFG::Visuals_Beams_Active);
				SliderFloat("Life Time", CFG::Visuals_Beams_LifeTime, 1.0f, 10.0f, 1.0f, "%.0fs");
				SliderFloat("Start Width", CFG::Visuals_Beams_Width, 1.0f, 10.0f, 1.0f, "%.0f");
				SliderFloat("End Width", CFG::Visuals_Beams_EndWidth, 1.0f, 10.0f, 1.0f, "%.0f");
				SliderFloat("Fade Length", CFG::Visuals_Beams_FadeLength, 1.0f, 10.0f, 1.0f, "%.0f");
				SliderFloat("Amplitude", CFG::Visuals_Beams_Amplitude, 0.0f, 10.0f, 0.1f, "%.1f");
				SliderFloat("Speed", CFG::Visuals_Beams_Speed, 0.0f, 10.0f, 1.0f, "%.0f");

				multiselect("Flags", BeamFlags, {
					{ "FBEAM_FADEIN", CFG::Visuals_Beams_Flag_FBEAM_FADEIN },
					{ "FBEAM_FADEOUT", CFG::Visuals_Beams_Flag_FBEAM_FADEOUT },
					{ "FBEAM_SINENOISE", CFG::Visuals_Beams_Flag_FBEAM_SINENOISE },
					{ "FBEAM_SOLID", CFG::Visuals_Beams_Flag_FBEAM_SOLID },
					{ "FBEAM_SHADEIN", CFG::Visuals_Beams_Flag_FBEAM_SHADEIN },
					{ "FBEAM_SHADEOUT", CFG::Visuals_Beams_Flag_FBEAM_SHADEOUT }
					});
			}
			GroupBoxEnd();
		}

		if (VisualsTab == EVisualsTabs::COLORS)
		{
			auto anchor_y{ m_nCursorY };

			GroupBoxStart("Menu", 150);
			{
				ColorPicker("Accent Primary", CFG::Menu_Accent_Primary);
				ColorPicker("Accent Secondary", CFG::Menu_Accent_Secondary);
				ColorPicker("Background", CFG::Menu_Background);
				CheckBox("Menu Snow", CFG::Menu_Snow);
			}
			GroupBoxEnd();

			GroupBoxStart("Visuals", 150);
			{
				ColorPicker("Hands", CFG::Color_Hands);
				ColorPicker("Hands Sheen", CFG::Color_Hands_Sheen);
				ColorPicker("Weapon", CFG::Color_Weapon);
				ColorPicker("Weapon Sheen", CFG::Color_Weapon_Sheen);
				ColorPicker("Projectile Arc", CFG::Color_Projectile_Arc);
			}
			GroupBoxEnd();

			m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
			m_nCursorY = anchor_y;

			GroupBoxStart("Visuals", 150);
			{
				ColorPicker("Local", CFG::Color_Local);
				ColorPicker("Friend", CFG::Color_Friend);
				ColorPicker("Enemy", CFG::Color_Enemy);
				ColorPicker("Teammate", CFG::Color_Teammate);
				ColorPicker("Target", CFG::Color_Target);
				ColorPicker("Invulnerable", CFG::Color_Invulnerable);
				ColorPicker("Cheater", CFG::Color_Cheater);
				ColorPicker("Retard Legit", CFG::Color_RetardLegit);
				ColorPicker("Invisible", CFG::Color_Invisible);
				ColorPicker("Over Heal", CFG::Color_OverHeal);
				ColorPicker("Uber", CFG::Color_Uber);
				ColorPicker("Conds", CFG::Color_Conds);
				ColorPicker("Health Pack", CFG::Color_HealthPack);
				ColorPicker("Ammo Pack", CFG::Color_AmmoPack);
				ColorPicker("Beams", CFG::Color_Beams);
				ColorPicker("Halloween Gifts", CFG::Color_Halloween_Gift);
				ColorPicker("MVM Money", CFG::Color_MVM_Money);
				ColorPicker("Particles", CFG::Color_Particles);
				ColorPicker("World Modulation", CFG::Color_World);
				ColorPicker("Sky Modulation", CFG::Color_Sky);
				ColorPicker("Prop Modulation", CFG::Color_Props);
			}
			GroupBoxEnd();
		}
	}
	
	if (MainTab == EMainTabs::MISC)
	{
		m_nCursorX += CFG::Menu_Spacing_X;

		int anchor_x = m_nCursorX;
		int anchor_y = m_nCursorY;

		GroupBoxStart("Misc", 160);
		{
			CheckBox("Bunnyhop", CFG::Misc_Bunnyhop);
			CheckBox("Choke on Bunnyhop", CFG::Misc_Choke_On_Bhop);
			CheckBox("Bypass sv_pure", CFG::Misc_Pure_Bypass);
			CheckBox("Noise Maker Spam", CFG::Misc_NoiseMaker_Spam);
			CheckBox("No Push", CFG::Misc_No_Push);
			CheckBox("Giant Weapon Sounds", CFG::Misc_MVM_Giant_Weapon_Sounds);
			CheckBox("Equip Region Unlock", CFG::Misc_Equip_Region_Unlock);
			CheckBox("Fast Stop", CFG::Misc_Fast_Stop);
			CheckBox("Anti Server Angle Change", CFG::Misc_Prevent_Server_Angle_Change);

			if (Button("Unlock CVars"))
			{
				auto iter = ICvar::Iterator(I::CVar);

				for (iter.SetFirst(); iter.IsValid(); iter.Next())
				{
					auto cmd = iter.Get();

					if (!cmd)
						continue;

					if (cmd->m_nFlags & FCVAR_DEVELOPMENTONLY)
						cmd->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;

					if (cmd->m_nFlags & FCVAR_HIDDEN)
						cmd->m_nFlags &= ~FCVAR_HIDDEN;

					if (cmd->m_nFlags & FCVAR_PROTECTED)
						cmd->m_nFlags &= ~FCVAR_PROTECTED;

					if (cmd->m_nFlags & FCVAR_CHEAT)
						cmd->m_nFlags &= ~FCVAR_CHEAT;
				}
			}
		}
		GroupBoxEnd();
		
		GroupBoxStart("Game", 160);
		{
			//at this point this does so much and lots of stuff relies on it, better make it impossible to turn off
			/*if (CheckBox("Accuracy Improvements", CFG::Misc_Accuracy_Improvements))
			{
				if (I::EngineClient->IsConnected())
				{
					I::EngineClient->ClientCmd_Unrestricted("retry");
				}
			}*/

			CheckBox("Network Fix", CFG::Misc_Ping_Reducer);
			CheckBox("Prediction Error Jitter Fix", CFG::Misc_Pred_Error_Jitter_Fix);
			CheckBox("ComputeLightingOrigin Fix", CFG::Misc_ComputeLightingOrigin_Fix);
			CheckBox("SetupBones Optimization", CFG::Misc_SetupBones_Optimization);
		}
		GroupBoxEnd();

		GroupBoxStart("Mann vs. Machine", 160);
		{
			InputKey("Instant Respawn", CFG::Misc_MVM_Instant_Respawn_Key);
			CheckBox("Instant Revive", CFG::Misc_MVM_Instant_Revive);
		}
		GroupBoxEnd();

		GroupBoxStart("Chat", 160);
		{
			CheckBox("Medieval", CFG::Misc_Chat_Medieval);
			CheckBox("OwO-ify", CFG::Misc_Chat_Owoify);
		}
		GroupBoxEnd();

		m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
		m_nCursorY = anchor_y;

		GroupBoxStart("Taunt", 150);
		{
			CheckBox("Taunt Slide", CFG::Misc_Taunt_Slide);
			CheckBox("Taunt Control", CFG::Misc_Taunt_Slide_Control);
			InputKey("Taunt Spin Key", CFG::Misc_Taunt_Spin_Key);
			SliderFloat("Taunt Spin Speed", CFG::Misc_Taunt_Spin_Speed, -50.0f, 50.0f, 1.0f, "%.0f");
			CheckBox("Taunt Spin Sine", CFG::Misc_Taunt_Spin_Sine);
			CheckBox("Fake Taunt", CFG::Misc_Fake_Taunt);
		}
		GroupBoxEnd();

		GroupBoxStart("Auto", 150);
		{
			CheckBox("Auto Disguise", CFG::Misc_Auto_Disguise);
			CheckBox("Auto Vaccinator", CFG::AutoVaccinator_Active);
			SelectSingle("Auto Vaccinator Pop", CFG::AutoVaccinator_Pop, {
				{ "Everyone", 0 },
				{ "Friends Only", 1 }
			});
			CheckBox("Auto Strafe", CFG::Misc_Auto_Strafe);
			SliderFloat("Auto Strafe Turn Scale", CFG::Misc_Auto_Strafe_Turn_Scale, 0.0f, 1.0f, 0.1f, "%.1f");
			InputKey("Auto RJ Key", CFG::Misc_Auto_Rocket_Jump_Key);
			InputKey("Auto AP Key", CFG::Misc_Auto_Air_Pogo_Key);
			InputKey("Auto Heal Key", CFG::Misc_Auto_Medigun_Key);
			InputKey("Undo Glue Key", CFG::Misc_Movement_Lock_Key);
			InputKey("Edge Jump Key", CFG::Misc_Edge_Jump_Key);
		}
		GroupBoxEnd();

		m_nCursorX += m_nLastGroupBoxW + (CFG::Menu_Spacing_X * 2);
		m_nCursorY = anchor_y;

		GroupBoxStart("Shifting", 150);
		{
			InputKey("Recharge Key", CFG::Exploits_Shifting_Recharge_Key);
			InputKey("Rapid Fire Key", CFG::Exploits_RapidFire_Key);
			SliderInt("Rapid Fire Ticks", CFG::Exploits_RapidFire_Ticks, 14, MAX_COMMANDS, 1);
			SliderInt("Rapid Fire Delay Ticks", CFG::Exploits_RapidFire_Min_Ticks_Target_Same, 0, 5, 1);
			CheckBox("Rapid Fire Antiwarp", CFG::Exploits_RapidFire_Antiwarp);
			InputKey("Warp Key", CFG::Exploits_Warp_Key);

			SelectSingle("Warp Mode", CFG::Exploits_Warp_Mode, {
				{ "Slow", 0 },
				{ "Full", 1 }
				});

			SelectSingle("Warp Exploit (for 'Full')", CFG::Exploits_Warp_Exploit, {
				{ "None", 0 },
				{ "Fake Peek", 1 },
				{ "0 Velocity", 2 }
				});

			CheckBox("Draw Indicator", CFG::Exploits_Shifting_Draw_Indicator);

			/*SelectSingle("Indicator Style", CFG::Exploits_Shifting_Indicator_Style,
			{
				{ "Rectangle", 0 },
				{ "Circle", 1 },
			});*/
		}
		GroupBoxEnd();

		GroupBoxStart("Crits", 150);
		{
			InputKey("Key", CFG::Exploits_Crits_Force_Crit_Key);
			InputKey("Melee Key", CFG::Exploits_Crits_Force_Crit_Key_Melee);
			CheckBox("Skip Random Crits", CFG::Exploits_Crits_Skip_Random_Crits);
		}
		GroupBoxEnd();

		GroupBoxStart("Seed Pred", 150);
		{
			CheckBox("Active", CFG::Exploits_SeedPred_Active);
			CheckBox("Draw Indicator", CFG::Exploits_SeedPred_DrawIndicator);
		}
		GroupBoxEnd();
	}

	if (MainTab == EMainTabs::PLAYERS)
	{
		m_nCursorX += CFG::Menu_Spacing_X;

		if (I::EngineClient->IsConnected())
		{
			for (auto n{ 1 }; n < I::EngineClient->GetMaxClients() + 1; n++)
			{
				if (n == I::EngineClient->GetLocalPlayer())
				{
					continue;
				}

				player_info_t player_info{};

				if (!I::EngineClient->GetPlayerInfo(n, &player_info) || player_info.fakeplayer)
				{
					continue;
				}

				players::PlayerInfo custom_info{};

				players::getInfo(n, custom_info);

				auto bx{ m_nCursorX };
				auto by{ m_nCursorY };

				if (custom_info.m_ignored)
				{
					playerListButton(Utils::ConvertUtf8ToWide(player_info.name).c_str(), 150, CFG::Color_Friend, false);
				}

				else if (custom_info.m_cheater)
				{
					playerListButton(Utils::ConvertUtf8ToWide(player_info.name).c_str(), 150, CFG::Color_Cheater, false);
				}

				else if (custom_info.m_retard_legit)
				{
					playerListButton(Utils::ConvertUtf8ToWide(player_info.name).c_str(), 150, CFG::Color_RetardLegit, false);
				}

				else
				{
					playerListButton(Utils::ConvertUtf8ToWide(player_info.name).c_str(), 150, CFG::Menu_Text_Inactive, false);
				}

				m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
				m_nCursorY = by;

				if (playerListButton(L"ignored", 60, custom_info.m_ignored ? CFG::Color_Friend : CFG::Menu_Text_Inactive, true))
				{
					players::mark(n, { !custom_info.m_ignored, false });
				}

				m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
				m_nCursorY = by;

				if (playerListButton(L"cheater", 60, custom_info.m_cheater ? CFG::Color_Cheater : CFG::Menu_Text_Inactive, true))
				{
					players::mark(n, { false, !custom_info.m_cheater });
				}

				m_nCursorX += m_nLastButtonW + CFG::Menu_Spacing_X;
				m_nCursorY = by;

				if (playerListButton(L"retard legit", 60, custom_info.m_retard_legit ? CFG::Color_RetardLegit : CFG::Menu_Text_Inactive, true))
				{
					players::mark(n, { false, false, !custom_info.m_retard_legit });
				}

				m_nCursorX = bx;
				m_nCursorY = by;

				m_nCursorY += H::Fonts->Get(EFonts::Menu).m_nTall + (CFG::Menu_Spacing_Y + 1);
			}
		}
	}

	if (MainTab == EMainTabs::CONFIGS)
	{
		static std::string strSelected = {};

		int nCount = 0;

		for (const auto &entry : std::filesystem::directory_iterator(m_strConfigPath))
		{
			if (std::string(std::filesystem::path(entry).filename().string()).find(".json") == std::string_view::npos)
				continue;

			nCount++;
		}

		if (nCount < 11)
		{
			std::string strInput = {};

			auto anchor_x{ m_nCursorX };
			auto anchor_y{ m_nCursorY };

			if (InputText("Create New", "Enter a Name:", strInput))
			{
				bool bAlreadyExists = [&]() -> bool
				{
					for (const auto &entry : std::filesystem::directory_iterator(m_strConfigPath))
					{
						if (std::string(std::filesystem::path(entry).filename().string()).find(".json") == std::string_view::npos)
							continue;

						if (!std::string(std::filesystem::path(entry).filename().string()).compare(strInput))
							return true;
					}

					return false;
				}();

				if (!bAlreadyExists)
					Config::Save((m_strConfigPath + "\\" + strInput + ".json").c_str());
			}
			
			//can't do this nicely after getting rid of std::any..

			/*auto anchor_x2{ anchor_x };

			m_nCursorX = anchor_x + m_nLastButtonW + CFG::Menu_Spacing_X;
			m_nCursorY = anchor_y;

			if (Button("Restore Defaults"))
			{
				for (const auto &var : Config::vecVarPtrs)
				{
					if (var->m_bNoSave)
					{
						continue;
					}

					var->m_Value = var->m_DefaultValue;
				}
			}

			m_nCursorX = anchor_x2;*/
		}

		if (strSelected.empty())
		{
			if (nCount > 0)
			{
				GroupBoxStart("Configs", 150);
				{
					m_nCursorY += CFG::Menu_Spacing_Y;

					for (const auto &entry : std::filesystem::directory_iterator(m_strConfigPath))
					{
						if (std::string(std::filesystem::path(entry).filename().string()).find(".json") == std::string_view::npos)
							continue;

						std::string s = entry.path().filename().string();
						s.erase(s.end() - 5, s.end());

						if (Button(s.c_str(), false, ((m_nLastGroupBoxW + 1) - (CFG::Menu_Spacing_X * 6))))
							strSelected = s;
					}
				}
				GroupBoxEnd();
			}
		}

		else
		{
			GroupBoxStart(strSelected.c_str(), 150);
			{
				m_nCursorY += CFG::Menu_Spacing_Y;

				int anchor_y = m_nCursorY;

				if (Button("Load")) {
					std::string full = (m_strConfigPath + "\\" + strSelected + ".json");
					Config::Load(full.c_str());
					strSelected = {};
				}

				m_nCursorX += 90;
				m_nCursorY = anchor_y;

				if (Button("Update")) {
					std::string full = (m_strConfigPath + "\\" + strSelected + ".json");
					Config::Save(full.c_str());
					strSelected = {};
				}

				if (Button("Delete")) {
					std::string full = (m_strConfigPath + "\\" + strSelected + ".json");
					std::filesystem::remove(full.c_str());
					strSelected = {};
				}

				if (Button("Cancel"))
					strSelected = {};

				m_nCursorX -= 90;
			}
			GroupBoxEnd();
		}
	}
}

void CMenu::Snow()
{
	struct SnowFlake_t
	{
		float m_flPosX = 0.0f;
		float m_flPosY = 0.0f;
		float m_flFallSpeed = 0.0f;
		float m_flDriftXSpeed = 0.0f;
		byte m_nAlpha{ 0 };
		int m_nSize{};
	};

	static std::vector<SnowFlake_t> vecSnowFlakes = {};

	if (!CFG::Menu_Snow)
	{
		if (!vecSnowFlakes.empty())
		{
			vecSnowFlakes.clear();
		}

		return;
	}

	auto GenerateSnowFlake = [](bool bFirstTime = false)
	{
		SnowFlake_t Out = {};

		Out.m_flPosX = static_cast<float>(Utils::RandInt(-(H::Draw->GetScreenW() / 2), H::Draw->GetScreenW()));
		Out.m_flPosY = static_cast<float>(Utils::RandInt(bFirstTime ? -(H::Draw->GetScreenH() * 2) : -100, -50));
		Out.m_flFallSpeed = static_cast<float>(Utils::RandInt(100, 200));
		Out.m_flDriftXSpeed = static_cast<float>(Utils::RandInt(10, 70));
		Out.m_nAlpha = static_cast<byte>(Utils::RandInt(5, 255));
		Out.m_nSize = Utils::RandInt(1, 2);

		return Out;
	};

	if (vecSnowFlakes.empty())
	{
		for (int n = 0; n < 1400; n++)
		{
			vecSnowFlakes.push_back(GenerateSnowFlake(true));
		}
	}

	for (auto &SnowFlake : vecSnowFlakes)
	{
		if (SnowFlake.m_flPosY > H::Draw->GetScreenH() + 50)
		{
			SnowFlake = GenerateSnowFlake();

			continue;
		}

		SnowFlake.m_flPosX += SnowFlake.m_flDriftXSpeed * I::GlobalVars->frametime;
		SnowFlake.m_flPosY += SnowFlake.m_flFallSpeed * I::GlobalVars->frametime;

		int nSize = SnowFlake.m_nSize;

		H::Draw->Rect(static_cast<int>(SnowFlake.m_flPosX), static_cast<int>(SnowFlake.m_flPosY), nSize, nSize, { 230, 230, 230, SnowFlake.m_nAlpha });
	}
}

void CMenu::Indicators()
{
	auto pLocal = H::Entities->GetLocal();

	int x = 2;
	int tall = H::Fonts->Get(EFonts::ESP_SMALL).m_nTall;
	int numitems = (pLocal && GetTFPlayerResource()) ? 3 : 2;
	int y = H::Draw->GetScreenH() - ((numitems * tall) + 2);
	int offset = 0;
	Color_t clr = { 200, 200, 200, 255 };

	H::Draw->String(H::Fonts->Get(EFonts::ESP_SMALL), x, y + (offset++ * tall), clr, POS_DEFAULT, "fps %d", static_cast<int>(1.0f / I::GlobalVars->absoluteframetime));

	if (auto pPR = GetTFPlayerResource())
	{
		if (pLocal)
		{
			H::Draw->String(H::Fonts->Get(EFonts::ESP_SMALL), x, y + (offset++ * tall), clr, POS_DEFAULT, "ping %d", pPR->GetPing(pLocal->entindex()));
		}
	}

	//H::Draw->String(H::Fonts->Get(EFonts::ESP_SMALL), x, y + (offset++ * tall), clr, POS_DEFAULT, "choked %d", I::ClientState->chokedcommands);
	H::Draw->String(H::Fonts->Get(EFonts::ESP_SMALL), x, y + (offset++ * tall), clr, POS_DEFAULT, "build %hs", __DATE__/* " " __TIME__*/);
}

void CMenu::Run()
{
	if (CFG::Misc_Clean_Screenshot && I::EngineClient->IsTakingScreenshot())
	{
		return;
	}

	if (!H::Input->IsGameFocused() && m_bOpen) {
		m_bOpen = false;
		I::MatSystemSurface->SetCursorAlwaysVisible(false);
		return;
	}

	if (!m_pGradient)
	{
		m_pGradient = std::make_unique<Color_t[]>(200 * 200);

		float hue = 0.0f, sat = 0.99f, lum = 1.0f;

		for (int i = 0; i < 200; i++)
		{
			for (int j = 0; j < 200; j++)
			{
				*reinterpret_cast<Color_t *>(m_pGradient.get() + j + i * 200) = ColorUtils::HSLToRGB(hue, sat, lum);
				hue += 1.0f / 200.0f;
			}

			lum -= 1.0f / 200.0f;
			hue = 0.0f;
		}

		m_nColorPickerTextureId = I::MatSystemSurface->CreateNewTextureID(true);
		I::MatSystemSurface->DrawSetTextureRGBAEx(m_nColorPickerTextureId, reinterpret_cast<const unsigned char *>(m_pGradient.get()), 200, 200, IMAGE_FORMAT_RGBA8888);
	}

	if (H::Input->IsPressed(VK_INSERT))
		I::MatSystemSurface->SetCursorAlwaysVisible(m_bOpen = !m_bOpen);

	Indicators();

	if (m_bOpen)
	{
		m_bClickConsumed = false;

		H::LateRender->Clear();

		MainWindow();

		H::LateRender->DrawAll();

		//rare cat
		{
			static bool is_running{ false };
			static float last_roll_time{ I::EngineClient->Time() };
			static float progress{ -30.0f };

			if (static_cast<int>(progress) > CFG::Menu_Width + 30)
			{
				is_running = false;
				progress = -30.0f;
			}

			if (!is_running && I::EngineClient->Time() - last_roll_time > 1.0f)
			{
				last_roll_time = I::EngineClient->Time();

				is_running = Utils::RandInt(0, 50) == 50;
			}

			if (is_running)
			{
				progress += 75.0f * I::GlobalVars->frametime;

				static float flLastFrameUpdateTime = I::EngineClient->Time();

				static int nFrame = 0;

				if (I::EngineClient->Time() - flLastFrameUpdateTime > 0.08f)
				{
					flLastFrameUpdateTime = I::EngineClient->Time();

					nFrame++;

					if (nFrame > 7)
					{
						nFrame = 0;
					}
				}

				H::Draw->StartClipping(CFG::Menu_Pos_X, 0, CFG::Menu_Width, H::Draw->GetScreenH());

				int offset{ 0 };

				if (nFrame == 1 || nFrame == 2 || nFrame == 3 || nFrame == 5 || nFrame == 6)
				{
					offset = 1;
				}

				//run test
				H::Draw->Texture
				(
					CFG::Menu_Pos_X + static_cast<int>(progress),
					CFG::Menu_Pos_Y - (13 + offset),
					20,
					13,
					F::VisualUtils->GetCatRun(nFrame),
					POS_DEFAULT
				);

				H::Draw->EndClipping();
			}
		}

		//cats idle
		{
			//idle left
			{
				static float flLastFrameUpdateTime = I::EngineClient->Time();

				static int nFrame = 0;

				if (I::EngineClient->Time() - flLastFrameUpdateTime > 0.2f)
				{
					flLastFrameUpdateTime = I::EngineClient->Time();

					nFrame++;

					if (nFrame > 3)
					{
						nFrame = 0;
					}
				}

				H::Draw->Texture(CFG::Menu_Pos_X + 5, CFG::Menu_Pos_Y - 12, 12, 12, F::VisualUtils->GetCat(nFrame), POS_DEFAULT);
			}

			//idle right
			{
				static float flLastFrameUpdateTime = I::EngineClient->Time();

				static int nFrame = 0;

				if (I::EngineClient->Time() - flLastFrameUpdateTime > 0.25f)
				{
					flLastFrameUpdateTime = I::EngineClient->Time();

					nFrame++;

					if (nFrame > 3)
					{
						nFrame = 0;
					}
				}

				H::Draw->Texture(CFG::Menu_Pos_X + 5 + 40, CFG::Menu_Pos_Y - 12, 12, 12, F::VisualUtils->GetCat2(nFrame), POS_DEFAULT);
			}

			//sleep
			{
				static float flLastFrameUpdateTime = I::EngineClient->Time();

				static int nFrame = 0;

				if (I::EngineClient->Time() - flLastFrameUpdateTime > 0.3f)
				{
					flLastFrameUpdateTime = I::EngineClient->Time();

					nFrame++;

					if (nFrame > 3)
					{
						nFrame = 0;
					}
				}

				H::Draw->Texture(CFG::Menu_Pos_X + 5 + 20, CFG::Menu_Pos_Y - 8, 12, 8, F::VisualUtils->GetCatSleep(nFrame), POS_DEFAULT);
			}
		}

		Snow();
	}
}

CMenu::CMenu()
{
	m_strConfigPath = std::filesystem::current_path().string() + "\\SEOwnedDE\\configs";
	
	if (!std::filesystem::exists(m_strConfigPath))
	{
		std::filesystem::create_directories(m_strConfigPath);
	}
}













































































//the piper never dies