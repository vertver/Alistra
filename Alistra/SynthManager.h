#pragma once
#include "NoteManager.h"
#include "EffectsClass.h"
#include "ADSR_Synthesizer.h"

class CBaseVoiceClass
{
private:
	i32 Note;
	f32 SampleRate;
	f32 fFrequency;
	f32 fVelocity;
	f32 fPhase;
	f32 fAddPhase;
	f32 fPrevTriangle;
	f32 fPulseWeight;
	f32 fNoiseValue;
	f64 b0, b1, b2;
	CADSREnvelope adsrEnvelope;
	SYNTH_STRUCT SynthStruct;

public:
	void Initialize(SYNTH_STRUCT* pSynthStruct, f32 fSampleRate)
	{
		adsrEnvelope.Initialize(&pSynthStruct->ADRSFirst, fSampleRate);
		memcpy(&SynthStruct, pSynthStruct, sizeof(SYNTH_STRUCT));
		SampleRate = fSampleRate;
	}

	void Reset()
	{
		fPhase = 0.f;
		fAddPhase = 0.f;
		fPrevTriangle = 0.f;
		fPulseWeight = 0.f;
		fNoiseValue = 0.f;
		b0 = 0.f;
		b1 = 0.f;
		b2 = 0.f;
	}

	void Update()
	{
		fAddPhase = fFrequency / SampleRate;
	}

	void NoteOn(i32 iNote, f32 Vel)
	{
		NoteOff();
		Note = iNote;
		fFrequency = GetMidiNoteFrequency(iNote);
		fVelocity = Vel;
	}

	void NoteOff()
	{
		fFrequency = 0.;
		fVelocity = 0.f;
		adsrEnvelope.Reset();
	}

	f32 NextSample()
	{
		f32 fRet = 0.f;

		f32 EndSample = 0.f;
		f32 EndSample2 = 0.f;
		f32 T = fPhase;
		f32 addPhase = fAddPhase;

		if (fFrequency >= 1.0)
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
				EndSample = addPhase * EndSample + (1.0f - addPhase) * fPrevTriangle;
				fPrevTriangle = EndSample;
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
				EndSample = (T < (0.5f + fPulseWeight * 0.25f)) ? 1.0f : -1.0f;
				EndSample += AntiAliasing(T, addPhase);
				EndSample -= AntiAliasing(fmodf(T + (1.0 - (0.5 + fPulseWeight * 0.25f)), 1.0), addPhase);
			}
			break;
			case 4:
			{
				f32 NoiseValue = fNoiseValue;
				NoiseValue += 19.f;
				NoiseValue *= NoiseValue;
				NoiseValue -= (i32)NoiseValue;
				fNoiseValue = NoiseValue;
				EndSample = NoiseValue - 0.5f;
			}
			break;
			case 5:
			{
				f32 NoiseValue = fNoiseValue;
				NoiseValue += 19.0f;
				NoiseValue *= NoiseValue;
				NoiseValue -= (i32)NoiseValue;
				fNoiseValue = NoiseValue;
				f64 RndValue = NoiseValue - 0.5f;
				b0 = 0.99765f * b0 + RndValue * 0.0990460f;
				b1 = 0.96300f * b1 + RndValue * 0.2965164f;
				b2 = 0.57000f * b2 + RndValue * 1.0526913f;
				EndSample = b0 + b1 + b2 + RndValue * 0.1848f;
			}
			break;
			default:
				EndSample = 0.f;
				break;
			}

			fPhase = fmodf(T + addPhase, 1.0f);
		}

		fRet = EndSample;
		return fRet;
	}
};

class CSynthManager
{
private:
	WAVE_FMT MixFmt;
	CNoteManager NoteManager;
	CEffectsClass EffectsClass;
	CBaseVoiceClass*** Voices;	

	NOTE_STRUCT NotesToUpdate[MAX_SYNTHS][MAX_POLY];

public:
	void Initialize(WAVE_FMT InFmt);
	void Destroy();

	void Process(float** pTemp, float** pBuffers, size_t Frames);
};
