/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once
#include "ADSR_Synthesizer.h"
#include "NoteManager.h"
#include "SoftClipper.h"
#include "EffectsClass.h"
#include "ChannelPanner.h"
#include "Automation.h"

class CDemoMixer
{
private:
	WAVE_FMT MixFmt;
	CNoteManager NoteManager;
	CSoftClipper Clipper;
	CEffectsClass Effects[MAX_SYNTHS];
	CChannelPanner Panner[MAX_SYNTHS];
	CFilterAutomation* pAutomation[MAX_SYNTHS];

public:
	CDemoMixer() 
	{
		for (size_t i = 0; i < MAX_SYNTHS; i++)
		{
			pAutomation[i] = nullptr;
		}
	}

	void Initialize(WAVE_FMT InFmt, AUTOMATION_STRUCT** pStruct, size_t* Automations);
	void Destroy();

	void Process(float** pTemp, float** pBuffers, size_t Frames);
};
