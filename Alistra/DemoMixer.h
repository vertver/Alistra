#pragma once
#include "ADSR_Synthesizer.h"
#include "NoteManager.h"

class CDemoMixer
{
private:
	size_t NowSynths;
	WAVE_FMT MixFmt;
	CEQFilter eqFilter;


public:
	void Initialize(WAVE_FMT InFmt);
	void Destroy();

	void Process(float** pTemp, float** pBuffers, size_t Frames);
};
