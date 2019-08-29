/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "Base_Synth.h"

void
CFadeOut::Process(
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
			fSign = (f32)signf(fRe);
			fRe = fabsf(fRe) - fTemp * u;
			fRe = fRe > 0 ? fRe * signf(fRe) : 0;
			pBuffers[i][u] = fRe;
		}
	}
}

