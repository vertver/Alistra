#include "DemoMixer.h"

void
CDemoMixer::Initialize(WAVE_FMT InFmt)
{

}

void
CDemoMixer::Destroy()
{

}

void
CDemoMixer::Process(float** pTemp, float** pBuffers, size_t Frames)
{
	for (size_t i = 0; i < NowSynths; i++)
	{
		i32 Note = 0;
		i32 Vel = 0;
		f32 Vol = 0.f;
		f32* pBufs[2] = {};
		size_t NowFrames = Frames;

		pBufs[0] = pTemp[0];
		pBufs[1] = pTemp[1];

		if (FramesToUpdate[i] && FramesToUpdate[i] < Frames)
		{
			Synthesis[i].Process(pBufs, FramesToUpdate[i]);
			Synthesis[i].NoteOff(i);

			/*
				Get pointer to next temp bytes
			*/
			pBufs[0] = &pBufs[0][FramesToUpdate[i]];
			pBufs[1] = &pBufs[1][FramesToUpdate[i]];

			NowFrames = Frames - FramesToUpdate[i];
			FramesToUpdate[i] = 0;
		}

		/*
			Next step for process
		*/
		if (!FramesToUpdate[i])
		{
			NoteManager.GetNextNote(i, Note, Vel, Vol, FramesToUpdate[i]);
		}

		Synthesis[i].Process(pBufs, NowFrames);
		FramesToUpdate[i] -= NowFrames;

		/*
			Mix final data
		*/
		for (size_t o = 0; o < 2; o++)
		{
			for (size_t u = 0; u < Frames; u++)
			{
				pBuffers[o][u] += pTemp[o][u];
			}		
		}
	}
}
 