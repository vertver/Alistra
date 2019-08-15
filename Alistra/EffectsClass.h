#pragma once
#include "Base_Sound.h"
#include "SoftClipper.h"
#include "ADSR_Synthesizer.h"

class CEffectsClass
{
private:
	f32 SampleRate;
	EFFECT_STRUCT EffectStruct;
	CEQFilter SimpleFilter[2];
	CSoftClipper SimpleClipper;
	CReverbEffect SimpleReverb;

public:
	void Initalize(EFFECT_STRUCT* pEffectStruct, f32 fSampleRate);
	void Destroy();

	void Process(float** pBuffers, size_t Frames);
};
