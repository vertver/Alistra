#include "ADSR_Synthesizer.h"

/*
f32
CADSRSynthesis::NextSample()
{	
	Voices.Update();
	f32 fRet = Voices.NextSample();	
	CurrentPosition++;

	return fRet;
}

void
CADSRSynthesis::Initialize(WAVE_FMT fmt, REVERB_STRUCT* pReverbStruct, FILTER_STRUCT* pFilterStruct, SYNTH_STRUCT* pSynthStruct)
{
	fSampleRate = (f32)fmt.SampleRate;

	eqFilter.Initialize(pFilterStruct);
	ReverbEffect.Initialize(pReverbStruct, fmt.SampleRate);
	Voices.Initialize(pSynthStruct, fmt.SampleRate);
}

void 
CADSRSynthesis::Process(f32** pBuffers, size_t Frames)
{
	for (size_t i = 0; i < Frames; i++)
	{
		f32 NewSample = NextSample();
		pBuffers[0][i] = NewSample;
		pBuffers[1][i] = NewSample;
	}

	eqFilter.Process(pBuffers, Frames);
	ReverbEffect.Process(pBuffers, Frames);
}

void
CADSRSynthesis::Reset()
{
	CurrentPosition = 0;

	eqFilter.Reset();
	ReverbEffect.Reset();
	Voices.Reset();

	memset(SynthBuffer, 0, sizeof(SynthBuffer));
}

void
CADSRSynthesis::NoteOn(i32 Note, f32 Vel, i32& Voice)
{
	Voices.NoteOn(Note, Vel, Voice);
}

void
CADSRSynthesis::NoteOff(i32 Voice) 
{
	Voices.NoteOff(Voice);
}
*/