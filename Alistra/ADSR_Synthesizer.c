#include "ADSR_Synthesizer.h"

SYNTH_STRUCT SynthsStruct[MAX_SYNTHS];
int SynthsCount = 0;

float pSynthBuffer[2][SYNTHBUFFER_SIZE];
LONGLONG CurrentPosition = -1;
size_t GlobalSampleRate = 0;

inline 
void
NextSampleInterval(
	MUSIC_INTERVAL* pCurrentInterval
)
{
	CurrentPosition++;


}

inline
void
ProcessSynthesis(
	SYNTH_STRUCT SynthStructure, 
	float** pBuffers, 
	size_t Frames
)
{
	for (size_t i = 0; i < Frames; i++)
	{
		MUSIC_INTERVAL ThisInterval = { 0 };
		NextSampleInterval(&ThisInterval);


	}
}

void
InitSynthesis(
	size_t gSampleRate
)
{
	GlobalSampleRate = gSampleRate;
}

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
}

void 
ResetSynth()
{
	for (size_t i = 0; i < 2; i++)
	{
		memset(pSynthBuffer[i], 0, SYNTHBUFFER_SIZE * sizeof(float));
	}
}

