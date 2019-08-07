#pragma once
#include "ADSR_Synthesizer.h"

void ProcessFadeOut(float** pBuffers, size_t Frames);

void ProcessFilter(FILTER_STRUCT* FilterStruct, size_t SynthIndex, float** pBuffers, size_t Frames);
void ResetFilter(size_t SynthIndex);

void InitReverb(REVERB_STRUCT* pReverbStruct, size_t SampleRate);
void ProcessReverb(float DryWet, float** pBuffers, size_t Frames);
void ResetReverb();
