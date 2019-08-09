#pragma once
#include "Base_Sound.h"
#include <math.h>

#define MAX_SYNTHS 24
#define SYNTHBUFFER_SIZE 1024

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
	float fReleaseCurve;
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

inline
float
GetMidiNoteFrequency(
	float Note
)
{
	/*
		Midi note to frequency
	*/
	return (8.1757989156437073336828122976022f * powf(1.0594630943592952645618252949463f, Note));
}

class CADSRSynthesis
{
private:
	size_t CurrentPosition = 0;
	float SynthBuffer[SYNTHBUFFER_SIZE];

	float NextSample();

public:
	void Initialize(SYNTH_STRUCT* pSynthStruct);
	void Process(float** pBuffers, size_t Frames);
	void Reset();
};
