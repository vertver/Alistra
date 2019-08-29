/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once
#include "Base_Sound.h"
#include "SoftClipper.h"
#include "ADSR_Synthesizer.h"
#include "Automation.h"

class CEffectsClass
{
private:
	bool isFirst;
	f32 SampleRate;
	EFFECT_STRUCT EffectStruct;
	CEQFilter SimpleFilter[2];
	CSoftClipper SimpleClipper;
	CReverbEffect SimpleReverb;
	CFilterAutomation Automation;

public:
	void Initalize(EFFECT_STRUCT* pEffectStruct, f32 fSampleRate);
	void Destroy();

	void SetFilterOptions(FILTER_STRUCT* pFilterOption);
	void Process(float** pBuffers, size_t Frames);
};
