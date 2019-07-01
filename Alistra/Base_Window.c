#include "Base.h"
#include "Base_Render.h"
#include "Base_Thread.h"

HWND MainHWND = NULL;
boolean Window_Flag_Resizeing = false;
THREAD_INFO tInfo;
HANDLE hCloseEvent = NULL;
HANDLE hWaitForInit = NULL;
HANDLE hThread = NULL;

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
				PostQuitMessage(0);
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
		Sleep(1);
		break;
	}

#ifdef DEBUG
	/*
		if we complete work with messages - give it to nuklear
	*/
	if (NuklearHandleEvent(hWnd, uMsg, wParam, lParam)) return 0;
#endif

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void 
CreateWindowProc(void* pParams)
{
	WNDCLASSW wc;
	RECT rect = { 0, 0, 640, 480 };
	DWORD style = WS_OVERLAPPEDWINDOW;
	DWORD exstyle = WS_EX_APPWINDOW;
	HWND wnd;

	/*
		Register our window class
	*/
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = CustomWindowProc;
	wc.hInstance = GetModuleHandleW(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
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

	MainHWND = wnd;

	/*
		Window process function, because we can
	*/
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

		Sleep(4);
	}
}

HWND GetMainWindowHandle()
{
	return MainHWND;
}

HWND CreateMainWindow()
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
	hThread = BaseCreateThread((ThreadFunc*)CreateWindowProc, &tInfo, 0);

	while (!MainHWND)
	{
		Sleep(1);
	}

	return MainHWND;
}

void DestroyMainWindow()
{
	/*
		We can't destroy NULL or invalid handle, right?
	*/
	if (MainHWND != NULL && MainHWND != INVALID_HANDLE_VALUE)
	{
		DestroyWindow(MainHWND);
		UnregisterClassW(L"ALISTRA_WINDOW", GetModuleHandle(0));

		if (hCloseEvent && hCloseEvent != INVALID_HANDLE_VALUE) CloseHandle(hCloseEvent);
		if (hWaitForInit && hWaitForInit != INVALID_HANDLE_VALUE) CloseHandle(hWaitForInit);
	}
}

void MainWindowLoop()
{

}