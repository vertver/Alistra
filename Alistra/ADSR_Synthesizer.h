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
	float fAttack;
	float fDecay;
	float fSustain;
	float fRelease;
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
} SYNTH_STRUCT;

typedef struct  
{
	float fTime;
	float fMix;
} REVERB_STRUCT;

typedef struct  
{
	float fFrequency;
	float fResonance;
} FILTER_STRUCT;

inline
float
GetNoteFrequency(
	float Note
)
{
	return (440 * powf(2, (Note - 69) / 12.0));
}

/*
	Use index of synth for multithread processing
*/
void AddSynth(SYNTH_STRUCT* pSynthStruct, int* pSynthIndex);

void ProcessSynth(int SynthIndex, float** pBuffers, size_t Frames);
void ResetSynth();
