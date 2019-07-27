#pragma once
#include "Base.h"

#define SIZEOF_NAME 256
#define MUSIC_BPM 140 
#define WHOLE_NOTES_COUNT 120
#define MAX_NOTES 6

typedef struct  
{
	DWORD StartFrame;
	DWORD EndFrame;
} TIME_INTERVAL;

typedef struct  
{	
	DWORD dwState;
	DWORD dwNoteIndex;
	float fVolumeLevel;
	float fInterval;
} NOTES_LIST;

typedef struct  
{	
	DWORD dwReserved;
	NOTES_LIST NotesArray[MAX_NOTES];
} MUSIC_INTERVAL;

typedef struct  
{
	BYTE Index;						// for sounds - index of audios, for devices - device index
	BYTE IsFloat;
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

boolean InitSound(char* OutputId, char* InputId);
void DestroySound();

boolean PlayAudio();
boolean StopAudio();

boolean EnumerateInputDevices(SOUNDDEVICE_INFO*** pSoundInfos, size_t* DevicesCount);
boolean EnumerateOutputDevices(SOUNDDEVICE_INFO*** pSoundInfos, size_t* DevicesCount);

boolean IsPlayingStarted();

/*
	Sound size in frames
*/
boolean CreateSoundWorker(boolean* pIsDoneBool);
void SoundWorkerProc(void* pParams);
float GetSoundWorkerProcess();
boolean IsSoundWorkerEnded();
boolean IsMusicEnd();

boolean ProcessSoundWorker(SOUNDDEVICE_INFO* pInfo);
void SoundWorker(float* FileData, size_t DataSize, int Channels);
void DestroySoundWorker();
