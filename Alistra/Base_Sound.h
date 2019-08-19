#pragma once
#include "Base.h"
#include <math.h>

#define signf(x)  (signbit(x) ?  -1 : 1)
#define SIZEOF_NAME 256
#define MUSIC_BPM 140.f
#define WHOLE_NOTES_COUNT 123.f
#define MAX_NOTES 6
#define MAX_AUTOMIZES 64
#define MAX_SYNTHS 10
#define MAX_POLY 16
#define MAX_DELAYBUFFERS 8
#define SYNTHBUFFER_SIZE 280
#define MAX_FRAME_SIZE SYNTHBUFFER_SIZE

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

inline
void
MixFramesToOutMix(f32* pMix, size_t MixChannels, f32* pOut, size_t OutChannels)
{
	if (OutChannels >= MixChannels)
	{
		if (MixChannels >= 2)
		{
			for (size_t i = 0; i < 2; i++)
			{
				pOut[i] = pMix[i];
			}
		}
		else
		{
			for (size_t i = 0; i < 2; i++)
			{
				pOut[i] = pMix[0];
			}
		}
	}
	else
	{
		if (MixChannels >= 2)
		{
			*pOut = (pMix[0] + pMix[1]) * 0.5f;
		}
		else
		{
			*pOut = *pMix;
		}
	}
}

inline
void
CopyMixToOut(f32** pMixStuff, f32* pOutStuff, size_t ChannelsMix, size_t ChannelsOut, size_t Frames)
{
	for (size_t i = 0; i < Frames; i++)
	{
		f32 ThisBuffer[2] = { pMixStuff[0][i], pMixStuff[1][i]};
		MixFramesToOutMix(ThisBuffer, ChannelsMix, &pOutStuff[i * ChannelsOut], ChannelsOut);
	}
}

typedef enum
{
	ESTAGE_ATTACK,
	ESTAGE_DECAY,
	ESTAGE_SUSTAIN,
	ESTAGE_RELEASE,
	ESTAGE_OFF
} ESTAGE;

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
	i32 iEnvelopeStage;
	f32 fEnvelopeLevel;
	f32 fAttack;
	f32 fDecay;
	f32 fSustain;
	f32 fRelease;
	f32 fAttackCurve;
	f32 fDecayReleaseCurve;
} ADSR_STRUCT;

typedef struct
{
	f32 fTime;
	f32 fFeedback;
	f32 fMix;
} REVERB_STRUCT;

typedef struct
{
	f32 fResonance;
	f32 fFrequency;
	size_t FilterType;
} FILTER_STRUCT;

typedef struct
{
	f32 fDrive;
	f32 fMix;
} CLIPPER_STRUCT;

typedef struct
{
	FILTER_STRUCT FilterSettings1;
	FILTER_STRUCT FilterSettings2;
	CLIPPER_STRUCT ClipperSettings;
	REVERB_STRUCT ReverbSettings;
} EFFECT_STRUCT;

typedef struct
{
	i32 SynthesisFirst;
	i32 SynthesisSecond;

	f32 fVibratoFirst;
	f32 fVibratoSecond;
	f32 fSynthStyleFirst;
	f32 fSynthStyleSecond;
	f32 fMix;					// 0.5 = 50% of first and 50% of second

	ADSR_STRUCT ADRSFirst;
	ADSR_STRUCT ADRSSecond;

	WAVE_FMT fmt;
} SYNTH_STRUCT;


typedef struct  
{
	f32 fPoint;
	size_t Frame;
} AUTOMATION_STRUCT;

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

void SetSoundExport();
bool IsSoundExported();
void GetExportPath(wchar_t* pOutpath);

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
	bool isFirst;
	float FloatTempArray1[4];
	float FloatTempArray2[4];
	FILTER_STRUCT FilterStruct;

public:
	void Initialize(FILTER_STRUCT* pFilterStruct);
	void SetOptions(FILTER_STRUCT* pFilterStruct);
	void Process(float** pBuffers, size_t Frames);
	void Reset();
};

class CDelay
{
private:
	u32 Position;
	u32 DelayLength;
	f32 fVolume;
	f32* pDelayBuffer;

public:
	CDelay() { Position = 0; DelayLength = 0; pDelayBuffer = nullptr; }
	void Initialize(u32 Length, f32 Volume);
	void Reset();	
	
	f32 Fetch();
	void Feed(f32 Value);
};

class CReverbEffect
{
private:
	f32 fSampleRate;
	i32 DelaySamples;
	i32 DelayBuffers;
	i32 Delays[MAX_DELAYBUFFERS];
	CDelay Delay[MAX_DELAYBUFFERS];
	REVERB_STRUCT ReverbStruct;

public:
	CReverbEffect() {}
	~CReverbEffect()
	{
		for (size_t i = 0; i < MAX_DELAYBUFFERS; i++)
		{
			Delay[i].Reset();
		}
	}

	void Initialize(REVERB_STRUCT* pReverbStruct, size_t SampleRate);
	void Process(float** pBuffers, size_t Frames);
	void Reset();
};
