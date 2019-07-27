#include "ADSR_Synthesizer.h"

SYNTH_STRUCT SynthsStruct[MAX_SYNTHS];
int SynthsCount = 0;

float pSynthBuffer[2][SYNTHBUFFER_SIZE];

void 
AddSynth(
	SYNTH_STRUCT* pSynthStruct,
	int* pSynthIndex
)
{
	if (SynthsCount < MAX_SYNTHS)
	{
		memcpy(&SynthsStruct[SynthsCount - 1], pSynthStruct, sizeof(SYNTH_STRUCT));
		*pSynthIndex = SynthsCount;
		SynthsCount++;
	}
}

void 
ProcessSynth(
	int SynthIndex,
	float** pBuffers, 
	size_t Frames
)
{
	for (size_t i = 0; i < Frames; i++)
	{

	}
}

void 
ResetSynth()
{
	for (size_t i = 0; i < 2; i++)
	{
		memset(pSynthBuffer[i], 0, SYNTHBUFFER_SIZE * sizeof(float));
	}
}
