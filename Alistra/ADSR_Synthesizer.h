#pragma once
#include "Base_Sound.h"
#include <math.h>

#define MAX_SYNTHS 24
#define MAX_POLY 6
#define SYNTHBUFFER_SIZE 1024

enum ESTAGE 
{
	ESTAGE_ATTACK,
	ESTAGE_DECAY,
	ESTAGE_SUSTAIN,
	ESTAGE_RELEASE,
	ESTAGE_OFF
};

class CADSREnvelope
{
private:
	f32 AttackBase;
	f32 AttackKoef;
	f32 DecayBase;
	f32 DecayKoef;
	f32 ReleaseBase;
	f32 ReleaseKoef;
	f32 SustainLevel;
	ADSR_STRUCT ADSRStruct;

public:
	void Initialize(ADSR_STRUCT* pADSRStruct, f32 SampleRate);
	void Reset();

	f32 NextEnvelope();
};

class CVoiceClass
{
private:
	CADSREnvelope adsrEnvelope[MAX_POLY];
	f32 SampleRate;
	f32 fPhase[MAX_POLY];
	f32 fAddPhase[MAX_POLY];
	f32 fPrevTriangle[MAX_POLY];
	f32 fPulseWeight[MAX_POLY];
	f32 fNoiseValue[MAX_POLY];
	f64 b0[MAX_POLY], b1[MAX_POLY], b2[MAX_POLY];
	i32 CountVoices;
	MUSIC_INTERVAL musIntervals;
	SYNTH_STRUCT SynthStruct;

public:
	void Initialize(SYNTH_STRUCT* pSynthStruct, f32 fSampleRate);
	void Reset();

	void Update();
	void NoteOn(i32 Note, f32 Vel, i32& Voice);
	void NoteOff(i32 Voice);

	f32 NextSample();
};

class CADSRSynthesis
{
private:
	CEQFilter eqFilter;
	CReverbEffect ReverbEffect;
	CVoiceClass Voices;
	size_t CurrentPosition;
	f32 fSampleRate;
	f32 SynthBuffer[SYNTHBUFFER_SIZE];

	f32 NextSample();

public:
	void NoteOn(i32 Note, f32 Vel, i32& Voice);
	void NoteOff(i32 Voice);

	void Initialize(WAVE_FMT fmt, REVERB_STRUCT* pReverbStruct, FILTER_STRUCT* pFilterStruct, SYNTH_STRUCT* pSynthStruct);
	void Process(f32** pBuffers, size_t Frames);
	void Reset();
};
