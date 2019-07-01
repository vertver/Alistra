#pragma once
#include "Base.h"

#define SIZEOF_NAME 256

typedef struct  
{
	DWORD StartFrame;
	DWORD EndFrame;
} TIME_INTERVAL;

typedef struct  
{
	BYTE Index;						// for sounds - index of audios, for devices - device index
	BYTE IsFloat;					// only for sounds, device always float
	BYTE Channels;
	BYTE Bits;						// only for sounds, device accept float values
	DWORD Frames;					// for sounds - frames of all audio sample, for devices - buffer size in frames
	DWORD SampleRate;			
	DWORD FadedSamples;				// only for sounds
	TIME_INTERVAL TimeInterval;		// only for sounds
} WAVE_FMT;

typedef struct  
{
	WAVE_FMT Fmt;
	char szName[SIZEOF_NAME];
	char szGUID[256];
} SOUNDDEVICE_INFO;

boolean InitSound();
void DestroySound();

boolean PlayAudio(char* DeviceId, DWORD dwSamples);
boolean StopAudio();

boolean EnumerateSoundDevices(SOUNDDEVICE_INFO*** pSoundInfos, size_t* DevicesCount);
