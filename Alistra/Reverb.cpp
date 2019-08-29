/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "Base_Synth.h"

void
CReverbEffect::Initialize(REVERB_STRUCT* pReverbStruct, size_t SampleRate)
{
	if (pReverbStruct->fMix > 0.f)
	{
		memcpy(&ReverbStruct, pReverbStruct, sizeof(REVERB_STRUCT));
		fSampleRate = (f32)SampleRate;
		f32 fDelayTime = (fSampleRate * (pReverbStruct->fTime / 1000.f));
		DelaySamples = (i32)fDelayTime;

		for (size_t i = 0; i < MAX_DELAYBUFFERS; i++)
		{
			Delays[i] = (i32)(fDelayTime + fDelayTime * (f32)i * 0.45f);
			Delay[i].Initialize(Delays[i], 1.0f - (((f32)i) * 0.09f));
		}
	}
}

void
CReverbEffect::Reset()
{
	memset(&ReverbStruct, 0, sizeof(REVERB_STRUCT));
	for (size_t i = 0; i < MAX_DELAYBUFFERS; i++)
	{
		Delay[i].Reset();
	}
}

void
CReverbEffect::Process(float** pBuffers, size_t Frames)
{
	if (ReverbStruct.fMix > 0.f)
	{
		f32 fMix = ReverbStruct.fMix;
		f32 fDryMix = fabsf(fMix - 1.0f);

		for (size_t i = 0; i < Frames; i++)
		{
			f32 fValue = pBuffers[0][i];
			f32 fDelayMix[MAX_DELAYBUFFERS] = {};
			f32 fDelay = 0;

			for (size_t o = 0; o < MAX_DELAYBUFFERS - 1; o++)
			{
				fDelayMix[o] = Delay[o].Fetch();
				fDelay += fDelayMix[o];
				Delay[o].Feed(fValue);
			}

			pBuffers[0][i] = fValue * fDryMix + fDelay * fMix;
			pBuffers[1][i] = fValue * fDryMix + fDelay * fMix;
		}
	}
}
 