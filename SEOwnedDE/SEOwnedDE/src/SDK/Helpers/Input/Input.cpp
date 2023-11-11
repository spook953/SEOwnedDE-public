#include "Input.h"

bool CInput::IsPressedAndHeld(short key)
{
	auto Now = std::chrono::steady_clock::now();

	static std::chrono::time_point<std::chrono::steady_clock> KeyTimes[256] = { Now };

	if (m_Keys[key] == PRESSED) {
		KeyTimes[key] = Now;
		return true;
	}

	if (m_Keys[key] == HELD && std::chrono::duration_cast<std::chrono::milliseconds>(Now - KeyTimes[key]).count() > 400)
		return true;

	return false;
}

void CInput::Update()
{
	static auto TF2Window = FindWindowA(0, "Team Fortress 2");

	m_bGameFocused = GetForegroundWindow() == TF2Window;

	for (int n = 0; n < 256; n++)
	{
		if (!m_bGameFocused) {
			m_Keys[n] = NONE;
			continue;
		}

		if (GetKeyState(n) & 0x8000)
		{
			if (m_Keys[n] == PRESSED)
				m_Keys[n] = HELD;

			else if (m_Keys[n] != HELD)
				m_Keys[n] = PRESSED;
		}

		else m_Keys[n] = NONE;
	}

	I::MatSystemSurface->SurfaceGetCursorPos(m_nMouseX, m_nMouseY);
}