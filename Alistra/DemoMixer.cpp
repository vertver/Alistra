#include "DemoMixer.h"

/*

	41.f, 40.0f, 7.0f, 0.5f, 0.2f
	maxroomsize, roomsize, revtime, damping, mix

	*******************************************

	26.0f, 25.0f, 4.5f, 0.5f, 0.2f
	26.0f, 25.0f, 4.5f, 0.5f, 0.2f
	20.0f, 19.0f, 1.12f, 0.5f, 0.35f
	0.f, 0.f, 0.f, 0.f, 0.f
	46.0f, 45.0f, 4.5f, 0.5f, 0.3f
	41.0f, 40.0f, 5.7f, 0.5f, 1.0f
	0.f, 0.f, 0.f, 0.f, 0.f
*/

const EFFECT_STRUCT AllEffectsForSynths[MAX_SYNTHS] =
{
	{	{ 0.5f, 0.35f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 26.0f, 25.0f, 4.5f, 0.5f, 0.2f }		},		// main arp
	{	{ 0.5f, 0.35f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 26.0f, 25.0f, 4.5f, 0.5f, 0.2f }		},		// second arp
	{	{ 0.4f, 0.5f, 0 },		{ 0.5f, 0.08f, 1 },		{ 0.2f,  1.0f	},		{ 20.0f, 19.0f, 1.12f, 0.5f, 0.35f }	},		// noise
	{	{ 0.6f, 0.5f, 0 },		{ 0.f, 0.f, 0 },		{ 0.15f, 1.0f	},		{ 0.f, 0.f, 0.f, 0.f, 0.f }				},		// bass
	{	{ 0.4f, 0.3f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 46.0f, 45.0f, 4.5f, 0.5f, 0.3f }		},		// pad  
	{	{ 0.4f, 0.3f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 41.0f, 40.0f, 5.7f, 0.5f, 1.0f }		},		// second pad  
	{	{ 0.4f, 0.5f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.15f, 1.0f	},		{ 0.f, 0.f, 0.f, 0.f, 0.f }				}		// kick
};

const PANNER_STRUCT AllPannerSettings[MAX_SYNTHS] =
{
	{ 0.45f, 1.0f },
	{ 0.55f, 0.92f },
	{ 0.5f, 0.60f },
	{ 0.5f, 0.50f },
	{ 0.58f, 0.15f },
	{ 0.42f, 0.15f },
	{ 0.5f, 1.0f }
};

void
CDemoMixer::Initialize(WAVE_FMT InFmt)
{
	CLIPPER_STRUCT ThisStruct = {};
	ThisStruct.fDrive = 0.f;
	ThisStruct.fMix = 1.f;

	NoteManager.Initialize(InFmt);
	Clipper.Initialize(&ThisStruct);

	for (size_t i = 0; i < MAX_SYNTHS; i++)
	{
		Effects[i].Initalize((EFFECT_STRUCT*)&AllEffectsForSynths[i], InFmt.SampleRate);
		Panner[i].Initialize((PANNER_STRUCT*)&AllPannerSettings[i]);
	}
}

void
CDemoMixer::Destroy()
{
	NoteManager.Destroy();
}

void
CDemoMixer::Process(float** pTemp, float** pBuffers, size_t Frames)
{
	for (size_t i = 0; i < MAX_SYNTHS; i++)
	{
		size_t NowFrames = Frames;

		NoteManager.Render(i, pTemp, Frames);
		Effects[i].Process(pTemp, Frames);
		Panner[i].Process(pTemp, Frames);

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
}
 