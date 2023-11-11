#include "WINAPI_WndProc.h"

#include "../Features/Menu/Menu.h"

LONG __stdcall Hooks::WINAPI_WndProc::Func(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (F::Menu->IsOpen() && H::Input->IsGameFocused())
	{
		if (GetAsyncKeyState(VK_SNAPSHOT) & 1) ///if (uMsg == WM_KEYUP && wParam == VK_SNAPSHOT) doesn't work
			CallWindowProc(Original, hWnd, uMsg, wParam, lParam);

		I::InputSystem->ResetInputState();

		return 1;
	}

	return CallWindowProc(Original, hWnd, uMsg, wParam, lParam);
}

void Hooks::WINAPI_WndProc::Init()
{
	Original = (WNDPROC)SetWindowLongPtr(hwWindow = FindWindowA(0, "Team Fortress 2"), GWL_WNDPROC, (LONG_PTR)Func);
}