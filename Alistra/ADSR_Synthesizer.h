/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once
#include "Base_Sound.h"
#include <math.h>

class CADSREnvelope
{
private:
	f32 AttackBase;
	f32 AttackKoef;
	f32 DecayBase;
	f32 DecayKoef;
	f32 ReleaseBase;
	f32 ReleaseKoef;
	f32 SustainLevel;
	ADSR_STRUCT ADSRStruct;

public:
	void Initialize(ADSR_STRUCT* pADSRStruct, f32 SampleRate);
	void Reset();

	void UpdateADSR(ADSR_STRUCT* pADSRStruct, f32 SampleRate);
	void BeginAttack();
	void BeginRelease();

	i32 GetCurrentState();

	f32 NextEnvelope();
};

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
	
	f32 FreqKoef = 3.1415f * 2.0f / 44100.0f;

	f32 KickFreq1 = 1000.f;
	f32 KickFreq2 = 158.f * 2.0f;
	f32 KickFreq3 = 58.f;

	f32 KickBodyMS = 1200.0f;
	f32 KickBodyFreq = 1.0 / (KickBodyMS * 0.001f);
	f32 KickLen = 44100.0f * (KickBodyMS * 0.001f);

	f32 KickMainMS = 80.0f;
	f32 KickMainLen = 44100.0f * (KickMainMS * 0.001f);
	f32 KickPhase = 0.0f;
	f32 KickFreq = KickFreq2;
	f32 Body = 1.0f;

	SYNTH_STRUCT SynthStruct;

public:
	void Initialize(SYNTH_STRUCT* pSynthStruct, f32 fSampleRate)
	{
		Reset();
		memcpy(&SynthStruct, pSynthStruct, sizeof(SYNTH_STRUCT));
		SampleRate = fSampleRate;
	}

	void Reset()
	{
		fPhase = 0.f;
		fAddPhase = 0.f;
		fPrevTriangle = 0.f;
		fPulseWeight = 0.f;
		fNoiseValue = 0.4f;
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
		KickPhase = 0.f;
		KickFreq = KickFreq2;
		Body = 1.0f;
	}

	i32 GetSynthType()
	{
		return SynthStruct.SynthesisFirst;
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
				EndSample += AntiAliasing(T, addPhase);
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
				EndSample = (f32)((f32)rand() / (f32)RAND_MAX);
			}
			break;
			case 5:
			{
				fNoiseValue += 19.0f;
				fNoiseValue *= fNoiseValue;
				fNoiseValue -= (i32)fNoiseValue;
				f64 RndValue = fNoiseValue - 0.5f;
				b0 = 0.99765f * b0 + RndValue * 0.0990460f;
				b1 = 0.96300f * b1 + RndValue * 0.2965164f;
				b2 = 0.57000f * b2 + RndValue * 1.0526913f;
				EndSample = b0 + b1 + b2 + RndValue * 0.1848f;
			}
			break;
			case 6:
			{
				f32 Out = sinf(KickPhase);

				KickPhase += FreqKoef * KickFreq;
				KickFreq += (KickFreq3 - KickFreq) * 0.001f * 2.0f;
				Out = fminf(1.0f, (sinf(Body * 3.1415f * 0.5f) * Out));
				if (Out > 0.5f) Out = (Out - 0.5f) * 0.5f + 0.5f;

				EndSample = 0.9 * Out;
				Body += (0 - Body) * (0.00009f) * 2.0f * 0.8f;
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

class CVoiceSynth
{
public:
	f32 SampleRate;
	i32 NotesOscs[MAX_POLY];
	SYNTH_STRUCT CurrentSynthType;
	CADSREnvelope adsrEnvelope[MAX_POLY];
	CBaseVoiceClass Oscillators[MAX_POLY];
	
	void Initialize(SYNTH_STRUCT* pSynthStruct, f32 fSampleRate);
	void Reset();

	void Process(f32** pBuffers, size_t Frames);

	void NoteOn(i32 Note, i32 Velocity);
	void NoteOff(i32 Note);
};
