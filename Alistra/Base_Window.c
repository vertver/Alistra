#include "Base.h"
#include "Base_Render.h"
#include "Base_Thread.h"
#include "Base_Sound.h"
#include "stdio.h"

boolean Window_Flag_Resizeing = false;
float fLoadProcess = 0.0f;
HANDLE hCloseEvent = NULL;
HANDLE hWaitForInit = NULL;
HANDLE hThread = NULL;
HWND MainHWND = NULL;
THREAD_INFO tInfo;

/*
	Events for our thread, because multithread code is so hard
*/
void
MainWindowDestroyEvent()
{
	if (hCloseEvent && hCloseEvent != INVALID_HANDLE_VALUE) SetEvent(hCloseEvent);
}

void
SetInitedEvent()
{
	if (hWaitForInit && hWaitForInit != INVALID_HANDLE_VALUE) SetEvent(hWaitForInit);
}

LRESULT
CALLBACK
CustomWindowProc(
	HWND   hWnd,
	UINT   uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_MOVE:
		Sleep(0);
		break;
	case WM_CLOSE:
		MainWindowDestroyEvent();
		break;
	case WM_COPY:
		break;
	case WM_ENTERSIZEMOVE:
		/*
			HACK: with this timer, we can draw window with new size
			while this window in resizing state
		*/
		SetTimer(hWnd, 2, 4, NULL);
		Window_Flag_Resizeing = true;
		return 0;
	case WM_EXITSIZEMOVE:
		KillTimer(hWnd, 2);
		Window_Flag_Resizeing = false;
		return 0;
	case WM_TIMER:
		RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INVALIDATE);
		return 0;
	case WM_PAINT:
		if (Window_Flag_Resizeing)
		{
			if (!RenderDraw())
			{
				return 0;
			}
		}
		break;
	case WM_SIZE:
		/*
			We can't resize to 0:0, and that's a reason why
			we don't put resize callback if minimized
		*/
		if (wParam != SIZE_MINIMIZED)
		{
			if (!ResizeRender(LOWORD(lParam), HIWORD(lParam)))
			{
				PostQuitMessage(0);
				return 0;
			}
		}
		else
		{
			Sleep(1);
		}
		return 0;
	case WM_SYSCOMMAND:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void 
DestroyMainWindow()
{
	/*
		We can't destroy NULL or invalid handle, right?
	*/
	if (MainHWND != NULL && MainHWND != INVALID_HANDLE_VALUE)
	{
		DestroyRender();
		DestroyWindow(MainHWND);
		UnregisterClassW(L"ALISTRA_WINDOW", GetModuleHandleW(NULL));

		if (hCloseEvent && hCloseEvent != INVALID_HANDLE_VALUE) CloseHandle(hCloseEvent);
		if (hWaitForInit && hWaitForInit != INVALID_HANDLE_VALUE) CloseHandle(hWaitForInit);
	}
}

void 
CreateWindowProc(
	void* pParams
)
{
	WNDCLASSW wc;
	RECT rect = { 0, 0, BASE_WIDTH, BASE_HEIGHT };
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	DWORD exstyle = WS_EX_APPWINDOW;
	HWND wnd;

	/*
		Register our window class
	*/
	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = CustomWindowProc;
	wc.hInstance = GetModuleHandleW(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(0);
	wc.lpszClassName = L"ALISTRA_WINDOW";
	RegisterClassW(&wc);

	AdjustWindowRectEx(&rect, style, FALSE, exstyle);

	/*
		Create window with custom caption
	*/
	wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"Alistra",
		style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, wc.hInstance, NULL
	);

	if (!wnd) ExitProcess(-132);

	MainHWND = wnd;

	while (WaitForSingleObject(hCloseEvent, 0) != WAIT_OBJECT_0)
	{
		MSG message;

		/*
			We can't do anything if we don't initialized
		*/
		WaitForSingleObject(hWaitForInit, INFINITE);

		/*
			Standart Windows message translate. No more.
		*/
		while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}

		/*
			Draw our application and wait for next time
		*/
		RenderDraw();
		
		/*
			Required by demoparty rules
		*/
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			break;
		}

		Sleep(4);
	}

	DestroyMainWindow();
	MainHWND = NULL;
}

HWND 
GetMainWindowHandle()
{
	return MainHWND;
}

HWND 
CreateMainWindow()
{
	/*
		Create close event
	*/
	if (hCloseEvent && hCloseEvent != INVALID_HANDLE_VALUE) CloseHandle(hCloseEvent);
	hCloseEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
	hWaitForInit = CreateEventW(NULL, TRUE, FALSE, NULL);

	memset(&tInfo, 0, sizeof(THREAD_INFO));

	/*
		Create window in other thread, because we don't want to
		run it at main thread
	*/
	hThread = BaseCreateThread((ThreadFunc*)CreateWindowProc, &tInfo, THREAD_PRIORITY_ABOVE_NORMAL);

	while (!MainHWND)
	{
		Sleep(1);
	}

	return MainHWND;
}

void
MainWindowLoop(
	boolean bAudio
)
{
	while (true)
	{
		Sleep(5);

		if (GetMainWindowHandle() == NULL)
		{
			break;
		}

		if (!bAudio)
		{
			if (IsMusicEnd())
			{
				break;
			}
		}
	}

	DestroySound();
}

void
SetLoadProcess(
	float floatProcess
)
{
	fLoadProcess = floatProcess;
}
