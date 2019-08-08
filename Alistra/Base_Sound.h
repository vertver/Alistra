#pragma once
#include "Base.h"

#define SIZEOF_NAME 256
#define MUSIC_BPM 140.f
#define WHOLE_NOTES_COUNT 123.f
#define MAX_NOTES 6

/*
	Music Beats = quarter note if 4/4. That means we can transform it
	to full whole notes (1/1).
*/
#define WHOLE_NOTES_PER_MINUTE (MUSIC_BPM / 4.f)

inline 
size_t
GetMusicFrames(size_t SampleRate)
{
	float FSRate = (float)SampleRate;
	float fTemp = ceilf(WHOLE_NOTES_COUNT / WHOLE_NOTES_PER_MINUTE * 60.f  * FSRate);

	return (size_t)fTemp;
}

typedef struct  
{
	float fResonance;
	float fFrequency;
	size_t FilterType;
} FILTER_STRUCT;

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

bool InitSound(char* OutputId, char* InputId);
void DestroySound();

bool PlayAudio();
bool StopAudio();

bool EnumerateInputDevices(SOUNDDEVICE_INFO*** pSoundInfos, size_t* DevicesCount);
bool EnumerateOutputDevices(SOUNDDEVICE_INFO*** pSoundInfos, size_t* DevicesCount);

bool IsPlayingStarted();

/*
	Sound size in frames
*/
bool CreateSoundWorker(boolean* pIsDoneBool);
void SoundWorkerProc(void* pParams);
float GetSoundWorkerProcess();
bool IsSoundWorkerEnded();
bool IsMusicEnd();

DWORD GetSampleRate();
bool ProcessSoundWorker(SOUNDDEVICE_INFO* pInfo);
void SoundWorker(float* FileData, size_t DataSize, int Channels);
void DestroySoundWorker();
