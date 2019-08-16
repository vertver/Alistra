#pragma once
#include "ADSR_Synthesizer.h"
#include "NoteManager.h"
#include "SoftClipper.h"
#include "EffectsClass.h"
#include "ChannelPanner.h"

class CDemoMixer
{
private:
	WAVE_FMT MixFmt;
	CNoteManager NoteManager;
	CSoftClipper Clipper;
	CEffectsClass Effects[MAX_SYNTHS];
	CChannelPanner Panner[MAX_SYNTHS];

public:
	void Initialize(WAVE_FMT InFmt);
	void Destroy();

	void Process(float** pTemp, float** pBuffers, size_t Frames);
};
