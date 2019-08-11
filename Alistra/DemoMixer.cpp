#include "DemoMixer.h"

void
CDemoMixer::Initialize(WAVE_FMT InFmt)
{

}

void
CDemoMixer::Destroy()
{

}

/*
	ƒоделать с нотами

	 ажда€ нота должна обновл€тьс€ каждый раз, когда она начинаетс€ и заканчиваетс€
*/
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
 