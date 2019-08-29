/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "EffectsClass.h"

void
CEffectsClass::Initalize(EFFECT_STRUCT* pEffectStruct, f32 fSampleRate)
{
	SampleRate = fSampleRate;
	memcpy(&EffectStruct, pEffectStruct, sizeof(EFFECT_STRUCT));

	SimpleFilter[0].Initialize(&pEffectStruct->FilterSettings1);
	SimpleFilter[1].Initialize(&pEffectStruct->FilterSettings2);
	SimpleClipper.Initialize(&pEffectStruct->ClipperSettings);
	SimpleReverb.Initialize(&pEffectStruct->ReverbSettings, SampleRate);
}

void
CEffectsClass::Destroy()
{
	SimpleClipper.Reset();
	SimpleFilter[0].Reset();
	SimpleFilter[1].Reset();
	SimpleReverb.Reset();
}

void
CEffectsClass::Process(float** pBuffers, size_t Frames)
{
	SimpleClipper.Process(pBuffers, Frames);	
	SimpleFilter[0].Process(pBuffers, Frames);
	SimpleFilter[1].Process(pBuffers, Frames);
	SimpleReverb.Process(pBuffers, Frames);
}

void
CEffectsClass::SetFilterOptions(FILTER_STRUCT* pFilterOption)
{
	SimpleFilter[0].SetOptions(pFilterOption);
}
