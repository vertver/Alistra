#pragma once
#include "Base.h"
#include <math.h>

#define signf(x)  (signbit(x) ?  -1 : 1)
#define SIZEOF_NAME 256
#define MUSIC_BPM 140.f
#define WHOLE_NOTES_COUNT 123.f
#define MAX_NOTES 6
#define MAX_AUTOMIZES 64

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

inline
float
fastatanf(float x)
{
	return (x / (1.0f + 0.28f * (x * x)));
}

inline
f64
GetMidiNoteFrequency(f32 Note)
{
	if (Note < 2.f) return 0.f;

	/*
		Midi note to frequency
	*/
	return (8.1757989156437073336828122976022f * pow(1.0594630943592952645618252949463f, (f64)Note));
}

inline
f32
AntiAliasing(f32 fTime, f32 fAdditionalPhase)
{
	f32 fRet = fTime;

	return 0.0f;
}

typedef struct
{
	DWORD StartFrame;
	DWORD EndFrame;
} TIME_INTERVAL;

typedef struct
{
	BYTE Index;						// for sounds - index of audios, for devices - device index
	BYTE IsFloat;
	BYTE Channels;
	BYTE Bits;						// only for sounds, device accept float values
	DWORD Frames;					// for sounds - frames of all audio sample, for devices - buffer size in frames
	DWORD SampleRate;
	TIME_INTERVAL TimeInterval;		// only for sounds
} WAVE_FMT;

/*
	2 OSC synthesizer with filter, effects and etc.
*/
typedef struct
{
	int iEnvelopeStage;
	float fEnvelopeLevel;
	float fAttack;
	float fDecay;
	float fSustain;
	float fRelease;
	float fAttackCurve;
	float fDecayReleaseCurve;
} ADSR_STRUCT;

typedef struct
{
	int SynthesisFirst;
	int SynthesisSecond;

	float fVibratoFirst;
	float fVibratoSecond;

	float fSynthStyleFirst;
	float fSynthStyleSecond;

	float fMix;					// 0.5 = 50% of first and 50% of second
	float fVolume;				// linear volume
	float fPan;

	ADSR_STRUCT ADRSFirst;
	ADSR_STRUCT ADRSSecond;

	WAVE_FMT fmt;
} SYNTH_STRUCT;

typedef struct
{
	float fRoomSize;
	float fFeedback;
	float fCrosstalk;
	float fMix;
} REVERB_STRUCT;

typedef struct  
{
	f32 fResonance;
	f32 fFrequency;
	size_t FilterType;
} FILTER_STRUCT;

typedef struct  
{
	float fPoint;
	size_t Frame;
} AUTOMIZE;

typedef struct  
{	
	DWORD dwState;
	f32 fVolumeLevel;
	f32 fInterval;
	f64 fNoteFreq;
} NOTES_LIST;

typedef struct  
{	
	DWORD dwReserved;
	NOTES_LIST NotesArray[MAX_NOTES];
} MUSIC_INTERVAL;

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

class CFadeOut
{
public:
	void Process(float** pBuffers, size_t Frames);
};

class CEQFilter
{
private:
	float FloatTempArray[4];
	FILTER_STRUCT FilterStruct;
	AUTOMIZE AutomizeStruct[MAX_AUTOMIZES];

public:
	void Initialize(FILTER_STRUCT* pFilterStruct);
	void Automize(AUTOMIZE* pAutomizeStructs, size_t StructsCount);
	void Process(float** pBuffers, size_t Frames);
	void Reset();
};

class CReverbEffect
{
private:
	REVERB_STRUCT ReverbStruct;
	AUTOMIZE AutomizeStruct[MAX_AUTOMIZES];

public:
	void Initialize(REVERB_STRUCT* pReverbStruct, size_t SampleRate);
	void Automize(AUTOMIZE* pAutomizeStructs, size_t StructsCount);
	void Process(float** pBuffers, size_t Frames);
	void Reset();
};
