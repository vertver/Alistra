#include "DemoSettings.h"

LRESULT
WINAPI
DemoWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	default:
		break;
	}

	return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

bool
OpenSettings()
{
	HWND hMainWindow = nullptr;
	HWND hParentWindow = nullptr;

	return false;
}

