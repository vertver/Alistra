#pragma once
#include "ADSR_Synthesizer.h"

void ProcessFadeOut(float** pBuffers, size_t Frames);

void ProcessReverb(REVERB_STRUCT* pReverbStruct, float** pBuffers, size_t Frames);
void ResetReverb();

void ProcessFilter(FILTER_STRUCT* pFilterStruct, float** pBuffers, size_t Frames);
void ResetFilter(FILTER_STRUCT* pFilterStruct, float** pBuffers, size_t Frames);
