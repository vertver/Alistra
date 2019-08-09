#include "Base_Synth.h"
#define signf(x)  (signbit(x) ?  -1 : 1)

float FloatTempArray[MAX_SYNTHS][4];

void 
ProcessFilter(
	FILTER_STRUCT* FilterStruct,
	size_t SynthIndex,
	float** pBuffers,
	size_t Frames
)
{
	float fTempValue = 0.f;
	float* pTempArray = FloatTempArray[SynthIndex];

	for (size_t i = 0; i < 2; i++)
	{
		float* pChannelBuffer = pBuffers[i];

		/*
			Fast and simple 24db/oct frequency filter 
		*/
		switch (FilterStruct->FilterType)
		{
		// Low pass filter
		case 0:		
			for (size_t u = 0; u < Frames; u++)
			{
				float fFreq = FilterStruct->fFrequency;
				float fFeedback = FilterStruct->fResonance * (1.0f + 1.0f / (1.0f - fFreq));

				fTempValue = pChannelBuffer[u];
				pTempArray[0] = fFreq * (fTempValue - pTempArray[0] + fFeedback * (pTempArray[0] - pTempArray[1]));
				pTempArray[1] = fFreq * (pTempArray[0] - pTempArray[1]);
				pTempArray[2] = fFreq * (pTempArray[1] - pTempArray[2]);
				pTempArray[3] = fFreq * (pTempArray[2] - pTempArray[3]);
				pChannelBuffer[u] = pTempArray[3];
			}
			break;
		// High pass filter
		case 1:
			for (size_t u = 0; u < Frames; u++)
			{
				float fFreq = FilterStruct->fFrequency;
				float fFeedback = FilterStruct->fResonance * (1.0f + 1.0f / (1.0f - fFreq));

				fTempValue = pChannelBuffer[u];
				pTempArray[0] = fFreq * (fTempValue - pTempArray[0] + fFeedback * (pTempArray[0] - pTempArray[1]));
				pTempArray[1] = fFreq * (pTempArray[0] - pTempArray[1]);
				pTempArray[2] = fFreq * (pTempArray[1] - pTempArray[2]);
				pTempArray[3] = fFreq * (pTempArray[2] - pTempArray[3]);
				pChannelBuffer[u] = pTempArray[0] - pTempArray[3];
			}
			break;
		// Band pass filter
		case 2:
			for (size_t u = 0; u < Frames; u++)
			{
				float fFreq = FilterStruct->fFrequency;
				float fFeedback = FilterStruct->fResonance * (1.0f + 1.0f / (1.0f - fFreq));

				fTempValue = pChannelBuffer[u];
				pTempArray[0] = fFreq * (fTempValue - pTempArray[0] + fFeedback * (pTempArray[0] - pTempArray[1]));
				pTempArray[1] = fFreq * (pTempArray[0] - pTempArray[1]);
				pChannelBuffer[u] = fTempValue - pTempArray[0];
			}
			break;
		}
	}
}

void
ResetFilter(size_t SynthIndex)
{
	float* pTempArray = FloatTempArray[SynthIndex];

	for (size_t i = 0; i < 4; i++)
	{
		pTempArray[i] = 0.f;
	}
}

void
ProcessFadeOut(
	float** pBuffers, 
	size_t Frames
)
{
	float fSign = 0.f;
	float fTemp = 0.f;
	float fOuts = 0.f;
	float fRe = 0.f;

	fTemp = 1.f / (float)Frames;

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t u = 0; u < Frames; u++)
		{
			fRe = pBuffers[i][u];
			fSign = signf(fRe);
			fRe = fabsf(fRe) - fTemp * u;
			fRe = fRe > 0 ? fRe * signf(fRe) : 0;
			pBuffers[i][u] = fRe;
		}
	}
}

