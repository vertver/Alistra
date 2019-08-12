#include "ADSR_Synthesizer.h"

void
CVoiceSynth::Initialize(SYNTH_STRUCT* pSynthStruct, f32 fSampleRate)
{	
	for (size_t i = 0; i < MAX_POLY; i++)
	{
		adsrEnvelope[i].Initialize(&pSynthStruct->ADRSFirst, fSampleRate);
		Oscillators[i].Initialize(pSynthStruct, fSampleRate);
		NotesOscs[i] = 0;
	}

	SampleRate = fSampleRate;
}

void
CVoiceSynth::Reset()
{
	for (size_t i = 0; i < MAX_POLY; i++)
	{
		Oscillators[i].Reset();
		adsrEnvelope[i].Reset();
	}	
}

void
CVoiceSynth::Process(f32** pBuffers, size_t Frames)
{
	for (size_t i = 0; i < MAX_POLY; i++)
	{
		for (size_t o = 0; o < Frames; o++)
		{
			f32 fOut = 0.f;
			Oscillators[i].Update();
			f32 fSample = Oscillators[i].NextSample();
			f32 fADSR = adsrEnvelope[i].NextEnvelope();

			if (fADSR < 0.0001f)
			{
				NotesOscs[i] = 0;
				Oscillators[i].NoteOff();
			}

			fOut += fSample * fADSR;

			pBuffers[0][o] += fOut;
			pBuffers[1][o] += fOut;		
		}
	}
}

void
CVoiceSynth::NoteOn(i32 Note, i32 Velocity)
{
	for (size_t i = 0; i < MAX_POLY; i++)
	{
		if (NotesOscs[i] < 2)
		{
			NotesOscs[i] = Note;
			Oscillators[i].NoteOn(Note, Velocity);
			adsrEnvelope[i].BeginAttack();
			break;
		}
	}
}

void
CVoiceSynth::NoteOff(i32 Note)
{
	for (size_t i = 0; i < MAX_POLY; i++)
	{
		if (NotesOscs[i] == Note)
		{
			adsrEnvelope[i].BeginRelease();
			break;
		}
	}
}
