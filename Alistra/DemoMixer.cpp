#include "DemoMixer.h"

void
CDemoMixer::Initialize(WAVE_FMT InFmt)
{
	CLIPPER_STRUCT ThisStruct = {};
	ThisStruct.fDrive = 0.f;
	ThisStruct.fMix = 1.f;

	NoteManager.Initialize(InFmt);
	Clipper.Initialize(&ThisStruct);
}

void
CDemoMixer::Destroy()
{
	NoteManager.Destroy();
}

void
CDemoMixer::Process(float** pTemp, float** pBuffers, size_t Frames)
{
#if 0
	for (size_t i = 0; i < MAX_SYNTHS; i++)
#else
	size_t i = 0;
#endif
	{
		size_t NowFrames = Frames;

		NoteManager.Render(i, pTemp, Frames);

		/*
			Mix final data
		*/
		for (size_t o = 0; o < 2; o++)
		{
			for (size_t u = 0; u < Frames; u++)
			{
				pBuffers[o][u] += (pTemp[o][u] * 0.25f);
			}
		}
	}

	Clipper.Process(pBuffers, Frames);
}
 