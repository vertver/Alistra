#include "EffectsClass.h"

void
CEffectsClass::Initalize(EFFECT_STRUCT* pEffectStruct, f32 fSampleRate)
{
	memcpy(&EffectStruct, pEffectStruct, sizeof(EFFECT_STRUCT));

	SimpleFilter.Initialize(&pEffectStruct->FilterSettings);
	SimpleClipper.Initialize(&pEffectStruct->ClipperSettings);
}

void
CEffectsClass::Destroy()
{

}

void
CEffectsClass::Process(float** pBuffers, size_t Frames)
{
	//SimpleClipper.Process(pBuffers, Frames);	
	SimpleFilter.Process(pBuffers, Frames);
}
