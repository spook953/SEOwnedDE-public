#pragma once

#include "../../SDK/SDK.h"

namespace Hooks
{
	namespace WINAPI_WndProc
	{
		inline HWND hwWindow;
		inline WNDPROC Original;
		LONG __stdcall Func(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void Init();
	}
}