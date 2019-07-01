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

	for (size_t i = 0; i < argc; i++)
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

  	if (!OpenGL_Library()) 
  	{ 
  		return -1; 
  	}
  	
  	int Funcs = 0;
 
	load_procs();

	SOUNDDEVICE_INFO** pIn_Info = NULL;
	SOUNDDEVICE_INFO** pOut_Info = NULL;
	size_t InCountDevices = 0;
	size_t OutCountDevices = 0;

	if (!bAudio)
	{
		if (!EnumerateOutputDevices(&pOut_Info, &OutCountDevices))
		{
			MessageBoxW(NULL, L"Can't enumerate devices. Please, restart demo with '-disable_audio' argument", L"ЕГГОГ", MB_OK | MB_ICONHAND);
			return -1;
		}

		if (!EnumerateInputDevices(&pIn_Info, &InCountDevices))
		{
			MessageBoxW(NULL, L"Can't enumerate devices. Please, restart demo with '-disable_audio' argument", L"ЕГГОГ", MB_OK | MB_ICONHAND);
			return -1;
		}

		/*
			Test zone
		*/
		{
			char szId[512];
			memset(szId, 0, sizeof(szId));

			for (size_t i = 0; i < OutCountDevices; i++)
			{
				sprintf_s(szId, 512, "%u - (%s)", pOut_Info[i]->Fmt.Index, pOut_Info[i]->szName);
				MessageBoxA(NULL, pOut_Info[i]->szGUID, szId, MB_OK);
				memset(szId, 0, sizeof(szId));
			}
		}

		{
			char szId[512];
			memset(szId, 0, sizeof(szId));

			for (size_t i = 0; i < InCountDevices; i++)
			{
				sprintf_s(szId, 512, "%u - (%s)", pIn_Info[i]->Fmt.Index, pIn_Info[i]->szName);
				MessageBoxA(NULL, pIn_Info[i]->szGUID, szId, MB_OK);
				memset(szId, 0, sizeof(szId));
			}
		}
	}

	HWND hwnd = CreateMainWindow();
	if (!hwnd) return -4;

	SetInitedEvent();

	while (true)
	{
		Sleep(1);

		if (GetMainWindowHandle() == NULL)
		{
			break;
		}

		DestroySound();
	}

	return Funcs;
}