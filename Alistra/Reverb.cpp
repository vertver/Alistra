#include "Base_Synth.h"


void
CReverbEffect::Initialize(REVERB_STRUCT* pReverbStruct, size_t SampleRate)
{
	if (pReverbStruct->fMix > 0.f)
	{
		memcpy(&ReverbStruct, pReverbStruct, sizeof(REVERB_STRUCT));
	}
}

void
CReverbEffect::Reset()
{
	memset(&ReverbStruct, 0, sizeof(REVERB_STRUCT));
}

void
CReverbEffect::Process(float** pBuffers, size_t Frames)
{
#if 0
	if (pReverb && ReverbStruct.fMix > 0.f)
	{
		f32 fMix = ReverbStruct.fMix;
		f32 fDryMix = fabsf(fMix - 1.0f);

		for (size_t i = 0; i < Frames; i++)
		{
			f32 fLeft = 0.f;
			f32 fRight = 0.f;
			f32 fValue = pBuffers[0][i];

			pBuffers[0][i] = fValue * fDryMix + fLeft * fMix;
			pBuffers[1][i] = fValue * fDryMix + fRight * fMix;
		}
	}
#endif
}
 