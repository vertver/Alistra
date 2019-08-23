#include "Base.h"
#include "Base_Sound.h"
#include "Base_Window.h"
#include "Base_Render.h"
#include "DemoSettings.h"
#include <stdio.h>
#include <cassert>

int
RealEntryPoint(
	char** argv,
	int argc
)
{
	bool bFullscreen = false;
	bool bVsync = false;
	bool isInited = false;
	bool isAudioWorkerDone = false;
	bool isImgui = false;
	bool bAudio = false;

#ifdef DEBUG
	for (int i = 0; i < argc; i++)
	{
		/*
			Ignore path to file. We don't need to process it
		*/
		if (!i) continue;
		if (strcmp(argv[i], "-use_imgui") == 0)
		{
			isImgui = true;
			continue;
		}

		if (strcmp(argv[i], "-disable_audio") == 0)
		{
			bAudio = true;
		}
	}
#endif

  	int Funcs = 0;

	BASE_OS_VERSION_INFO pOsInfo;
	memset(&pOsInfo, 0, sizeof(BASE_OS_VERSION_INFO));
	if (!GetSystemVersion(&pOsInfo))
	{
		return -32;
	}

	if (!bAudio)
	{
		SOUNDDEVICE_INFO** pIn_Info = NULL;
		SOUNDDEVICE_INFO** pOut_Info = NULL;
		size_t InCountDevices = 0;
		size_t OutCountDevices = 0;	

		if (!EnumerateOutputDevices(&pOut_Info, &OutCountDevices) || !EnumerateInputDevices(&pIn_Info, &InCountDevices))
		{
#ifdef DEBUG
			MessageBoxW(NULL, L"Can't enumerate devices. Please, restart demo with '-disable_audio' argument", L"Ð•Ð“Ð“ÐžÐ“", MB_OK | MB_ICONHAND);	
			return -1;
#else
			MessageBoxA(NULL, "Can't enumerate devices. Starting 'Alistra' without sound", "ÂÀÐÍÈÍÃ“", MB_OK | MB_ICONWARNING);
			bAudio = true;
#endif
		}
	}

#ifdef DEBUG
	if (!OpenSettings(bVsync, bFullscreen)) return false;
#endif

	HWND hwnd = CreateMainWindow(0, 0);
	if (!hwnd) return -4;

	/*
		Init all stuff
	*/
    bool render = InitRender(isImgui, bVsync, bFullscreen);
	SetInitedEvent();
	SetSoundExport();

	if (!bAudio)
	{
		if (!InitSound(NULL, NULL))
		{
			MessageBoxA(NULL, "Can't create sound device. Starting 'Alistra' without sound", "ÅÃÃÎÃ", MB_OK | MB_ICONWARNING);
			bAudio = true;
		}

		if (!CreateSoundWorker(&isAudioWorkerDone))
		{
			bAudio = true;
		}
	}

	/*
		Load process loop. We are here while our demo is not initialized
	*/
	while (!isInited)
	{
		bool isEnd;

		if (!bAudio)
		{
			isEnd = (IsSoundWorkerEnded() && IsRenderWorkDone());
				
			SetLoadProcess((GetSoundWorkerProcess()));
		}
		else
		{
			isEnd = IsRenderWorkDone();
			SetLoadProcess(GetRenderLoadProcess());
		}

		isInited = isEnd;
		Sleep(5);
	}

	if (isAudioWorkerDone)
	{
		PlayAudio();

		/*
			Waiting for starting WASAPI thread
		*/
		while (!IsPlayingStarted())
		{
			Sleep(1);
		}
	}

	/*
		While application is not closed - we are here
	*/
	MainWindowLoop(bAudio);

	return Funcs;
}
