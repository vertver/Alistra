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
	boolean isInited = false;
	boolean isAudioWorkerDone = false;
	boolean isImgui = false;
	boolean bAudio = false;

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
			MessageBoxW(NULL, L"Can't enumerate devices. Please, restart demo with '-disable_audio' argument", L"ЕГГОГ", MB_OK | MB_ICONHAND);	
			return -1;
#else
			MessageBoxW(NULL, L"Can't enumerate devices. Starting 'Alistra' without sound", L"ВАРНИНГ", MB_OK | MB_ICONWARNING);
			bAudio = true;
#endif
		}
	}

	HWND hwnd = CreateMainWindow();
	if (!hwnd) return -4;

	/*
		Init all stuff
	*/
	InitRender(isImgui);
	SetInitedEvent();

	if (!bAudio)
	{
		if (!InitSound(NULL, NULL))
		{
			MessageBoxW(NULL, L"Can't create sound device. Starting 'Alistra' without sound", L"ВАРНИНГ", MB_OK | MB_ICONWARNING);
			bAudio = true;
		}

		if (!CreateSoundWorker(&isAudioWorkerDone))
		{
			bAudio = true;
		}
	}

	/*
		Load process loop. We are here while our demo is not initiated
	*/
	while (!isInited)
	{
		boolean isEnd;

		if (!bAudio)
		{
			isEnd = (IsSoundWorkerEnded() && IsRenderWorkDone());
				
			SetLoadProcess((GetSoundWorkerProcess() * 0.5f) + (GetRenderLoadProcess() * 0.5f));
		}
		else
		{
			isEnd = IsRenderWorkDone();
			SetLoadProcess((GetRenderLoadProcess() * 0.5f));
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
