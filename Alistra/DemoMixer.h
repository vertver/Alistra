#pragma once
#include "ADSR_Synthesizer.h"
#include "NoteManager.h"

class CDemoMixer
{
private:
	size_t NowSynths;
	WAVE_FMT MixFmt;
	CNoteManager NoteManager;
	CEQFilter eqFilter;
	CADSRSynthesis Synthesis[MAX_SYNTHS];
	size_t FramesToUpdate[MAX_SYNTHS];


public:
	void Initialize(WAVE_FMT InFmt);
	void Destroy();

	void Process(float** pTemp, float** pBuffers, size_t Frames);
};
