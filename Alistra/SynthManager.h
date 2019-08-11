#pragma once
#include "NoteManager.h"
#include "EffectsClass.h"
#include "ADSR_Synthesizer.h"

class CBaseVoiceClass
{
private:
	i32 Note;
	f32 SampleRate;
	f32 fFrequency;
	f32 fVelocity;
	f32 fPhase;
	f32 fAddPhase;
	f32 fPrevTriangle;
	f32 fPulseWeight;
	f32 fNoiseValue;
	f64 b0, b1, b2;
	CADSREnvelope adsrEnvelope;
	SYNTH_STRUCT SynthStruct;

public:
	void Initialize(SYNTH_STRUCT* pSynthStruct, f32 fSampleRate);
	void Reset();

	void Update();
	void NoteOn(i32 iNote, f32 Vel);
	void NoteOff();

	f32 NextSample();
};

class CSynthManager
{
private:
	WAVE_FMT MixFmt;
	CNoteManager NoteManager;
	CEffectsClass EffectsClass;
	CBaseVoiceClass*** Voices;	

	NOTE_STRUCT NotesToUpdate[MAX_SYNTHS][MAX_POLY];

public:
	void Initialize(WAVE_FMT InFmt);
	void Destroy();

	void Process(float** pTemp, float** pBuffers, size_t Frames);
};
