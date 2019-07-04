#include "Base.h"
#include "Base_Sound.h"
#include "Base_Window.h"
#include "Base_Render.h"
#include <stdio.h>

int
RealEntryPoint(
	char** argv,
	int argc
)
{
	boolean isImgui = false;
	boolean bAudio = false;

	for (int i = 0; i < argc; i++)
	{
		/*
			Ignore path to file. We don't need to process it
		*/
		if (!i) continue;
		if (strcmp(argv[i], "-use_imgui"))
		{
			isImgui = true;
			continue;
		}

		if (strcmp(argv[i], "-disable_audio"))
		{
			bAudio = true;
			continue;
		}
	}
  	
  	int Funcs = 0;

	BASE_OS_VERSION_INFO* pOsInfo;
	pOsInfo = HeapAlloc(GetProcessHeap(), 0, sizeof(BASE_OS_VERSION_INFO));
	memset(pOsInfo, 0, sizeof(BASE_OS_VERSION_INFO));
	if (!GetSystemVersion(pOsInfo)) return -32;

	if (!bAudio)
	{
		SOUNDDEVICE_INFO** pIn_Info = NULL;
		SOUNDDEVICE_INFO** pOut_Info = NULL;
		size_t InCountDevices = 0;
		size_t OutCountDevices = 0;	

		if (!EnumerateOutputDevices(&pOut_Info, &OutCountDevices) || !EnumerateInputDevices(&pIn_Info, &InCountDevices))
		{
			MessageBoxW(NULL, L"Can't enumerate devices. Please, restart demo with '-disable_audio' argument", L"ЕГГОГ", MB_OK | MB_ICONHAND);
			return -1;
		}
	}

	HWND hwnd = CreateMainWindow();
	if (!hwnd) return -4;

	/*
		Init all stuff
	*/
	InitRender(isImgui);
	SetInitedEvent();

	/*
		While application is not closed - we are here
	*/
	MainWindowLoop();

	return Funcs;
}