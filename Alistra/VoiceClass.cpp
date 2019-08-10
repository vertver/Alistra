#include "ADSR_Synthesizer.h"

/*
	Original synth code written by Vladimir Shatrov https://github.com/frowrik
*/

void
CVoiceClass::Initialize(SYNTH_STRUCT* pSynthStruct, f32 fSampleRate)
{
	Reset();

	adsrEnvelope->Initialize(&pSynthStruct->ADRSFirst);
	memcpy(&SynthStruct, pSynthStruct, sizeof(SYNTH_STRUCT));
	SampleRate = fSampleRate;
}

void
CVoiceClass::Reset()
{
	for (size_t i = 0; i < MAX_POLY; i++)
	{
		fPhase[i] = 0.f;
		fAddPhase[i] = 0.f;
		fPrevTriangle[i] = 0.f;
		fPulseWeight[i] = 0.f;
		fNoiseValue[i] = 0.f;
		b0[i] = 0.f;
		b1[i] = 0.f;
		b2[i] = 0.f;
	}
}

void
CVoiceClass::Update()
{
	for (size_t i = 0; i < MAX_POLY; i++)
	{
		if (musIntervals.NotesArray[i].fNoteFreq >= 1.0)
		{
			float fPhaseTemp = musIntervals.NotesArray[i].fNoteFreq / SampleRate;
		}
	}
}

void
CVoiceClass::NoteOn(i32 Note, f32 Vel, i32& Voice)
{
	i32 indexCurrent = 1;
	if (CountVoices >= MAX_POLY) return;

	for (size_t i = 0; i < MAX_POLY; i++)
	{
		if (musIntervals.NotesArray[i].fNoteFreq < 1.0)
			indexCurrent++;
		else
			break;
	}

	musIntervals.NotesArray[indexCurrent].fNoteFreq = GetMidiNoteFrequency(Note);
	musIntervals.NotesArray[indexCurrent].fInterval = Vel;
	CountVoices++;
}

void
CVoiceClass::NoteOff(i32 Voice)
{
	if (CountVoices)
	{
		musIntervals.NotesArray[Voice].fNoteFreq = 0.;
		musIntervals.NotesArray[Voice].fInterval = 0.f;
		adsrEnvelope[Voice].Reset();
		CountVoices--;
	}
}

f32
CVoiceClass::NextSample()
{
	f32 fRet = 0.f;

	for (size_t i = 0; i < MAX_POLY; i++)
	{
		f32 EndSample = 0.f;
		f32 EndSample2 = 0.f;
		f32 T = fPhase[i];
		f32 addPhase = fAddPhase[i];

		if (musIntervals.NotesArray[i].fNoteFreq >= 1.0)
		{
			switch (SynthStruct.SynthesisFirst)
			{
			case 0:
			{
				EndSample = sinf(T * 6.283185307179586476925286766559005f);
			}
			break;
			case 1:
			{
				EndSample = (T < 0.5f) ? 1.0f : -1.0f;
				EndSample += AntiAliasing(T, addPhase);
				EndSample -= AntiAliasing(fmodf(T + 0.5f, 1.0f), addPhase);
				EndSample = addPhase * EndSample + (1.0f - addPhase) * fPrevTriangle[i];
				fPrevTriangle[i] = EndSample;
				EndSample *= 5.f;
			}
			break;
			case 2:
			{
				EndSample = 1.0f - 2.0f * T;
				EndSample = AntiAliasing(T, addPhase);
			}
			break;
			case 3:
			{
				EndSample = (T < (0.5f + fPulseWeight[i] * 0.25f)) ? 1.0f : -1.0f;
				EndSample += AntiAliasing(T, addPhase);
				EndSample -= AntiAliasing(fmodf(T + (1.0 - (0.5 + fPulseWeight[i] * 0.25f)), 1.0), addPhase);
			}
			break;
			case 4:
			{
				f32 NoiseValue = fNoiseValue[i];
				NoiseValue += 19.f;
				NoiseValue *= NoiseValue;
				NoiseValue -= (i32)NoiseValue;
				fNoiseValue[i] = NoiseValue;
				EndSample = NoiseValue - 0.5f;
			}
			break;
			case 5:
			{
				f32 NoiseValue = fNoiseValue[i];
				NoiseValue += 19.0f;
				NoiseValue *= NoiseValue;
				NoiseValue -= (i32)NoiseValue;
				fNoiseValue[i] = NoiseValue;
				f64 RndValue = NoiseValue - 0.5f;
				b0[i] = 0.99765f * b0[i] + RndValue * 0.0990460f;
				b1[i] = 0.96300f * b1[i] + RndValue * 0.2965164f;
				b2[i] = 0.57000f * b2[i] + RndValue * 1.0526913f;
				EndSample = b0[i] + b1[i] + b2[i] + RndValue * 0.1848f;
			}
			break;
			default:
				EndSample = 0.f;
			break;
			}

			fPhase[i] = fmodf(T + addPhase, 1.0f);
		}

		fRet += EndSample;
	}

	return fRet;
}
