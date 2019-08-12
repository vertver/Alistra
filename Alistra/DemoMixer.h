#pragma once
#include "ADSR_Synthesizer.h"
#include "NoteManager.h"
#include "SoftClipper.h"

class CDemoMixer
{
private:
	WAVE_FMT MixFmt;
	CNoteManager NoteManager;
	CSoftClipper Clipper;

public:
	void Initialize(WAVE_FMT InFmt);
	void Destroy();

	void Process(float** pTemp, float** pBuffers, size_t Frames);
};
