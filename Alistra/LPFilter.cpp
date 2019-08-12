#include "Base_Synth.h"

void
CEQFilter::Initialize(FILTER_STRUCT* pFilterStruct)
{
	for (size_t i = 0; i < 4; i++)
	{
		FloatTempArray1[i] = 0.f;
		FloatTempArray2[i] = 0.f;
	}

	memcpy(&FilterStruct, pFilterStruct, sizeof(FILTER_STRUCT));
}

void 
CEQFilter::Process(
	float** pBuffers,
	size_t Frames
)
{
	float fTempValue = 0.f;

	for (size_t i = 0; i < 2; i++)
	{	
		float* pTempArray = nullptr;
		float* pChannelBuffer = pBuffers[i];

		if (i == 0)
		{
			pTempArray = FloatTempArray1;
		}
		else if (i == 1)
		{
			pTempArray = FloatTempArray2;
		}

		/*
			Fast and simple 24db/oct frequency filter 
		*/
		switch (FilterStruct.FilterType)
		{
		// Low pass filter
		case 0:		
			for (size_t u = 0; u < Frames; u++)
			{
				float fFreq = FilterStruct.fFrequency;
				float fFeedback = FilterStruct.fResonance * (1.0f + 1.0f / (1.0f - fFreq));

				fTempValue = pChannelBuffer[u];
				pTempArray[0] += fFreq * (fTempValue - pTempArray[0] + fFeedback * (pTempArray[0] - pTempArray[1]));
				pTempArray[1] += fFreq * (pTempArray[0] - pTempArray[1]);
				pTempArray[2] += fFreq * (pTempArray[1] - pTempArray[2]);
				pTempArray[3] += fFreq * (pTempArray[2] - pTempArray[3]);
				pChannelBuffer[u] = pTempArray[3];
			}
			break;
		// High pass filter
		case 1:
			for (size_t u = 0; u < Frames; u++)
			{
				float fFreq = FilterStruct.fFrequency;
				float fFeedback = FilterStruct.fResonance * (1.0f + 1.0f / (1.0f - fFreq));

				fTempValue = pChannelBuffer[u];
				pTempArray[0] += fFreq * (fTempValue - pTempArray[0] + fFeedback * (pTempArray[0] - pTempArray[1]));
				pTempArray[1] += fFreq * (pTempArray[0] - pTempArray[1]);
				pTempArray[2] += fFreq * (pTempArray[1] - pTempArray[2]);
				pTempArray[3] += fFreq * (pTempArray[2] - pTempArray[3]);
				pChannelBuffer[u] = pTempArray[0] - pTempArray[3];
			}
			break;
		// Band pass filter
		case 2:
			for (size_t u = 0; u < Frames; u++)
			{
				float fFreq = FilterStruct.fFrequency;
				float fFeedback = FilterStruct.fResonance * (1.0f + 1.0f / (1.0f - fFreq));

				fTempValue = pChannelBuffer[u];
				pTempArray[0] += fFreq * (fTempValue - pTempArray[0] + fFeedback * (pTempArray[0] - pTempArray[1]));
				pTempArray[1] += fFreq * (pTempArray[0] - pTempArray[1]);
				pChannelBuffer[u] = fTempValue - pTempArray[0];
			}
			break;
		}
	}
}

void
CEQFilter::Reset()
{
	for (size_t i = 0; i < 4; i++)
	{
		FloatTempArray1[i] = 0.f;
		FloatTempArray2[i] = 0.f;
	}

	memset(&FilterStruct, 0, sizeof(FilterStruct));
}
