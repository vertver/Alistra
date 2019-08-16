#include "DemoMixer.h"

const EFFECT_STRUCT AllEffectsForSynths[MAX_SYNTHS] =
{
	{	{ 0.5f, 0.35f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 0.3f, 0.5f, 0.2f, 0.2f }	},		// main arp
	{	{ 0.5f, 0.35f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 0.3f, 0.5f, 0.2f, 0.2f }	},		// second arp
	{	{ 0.4f, 0.5f, 0 },		{ 0.5f, 0.08f, 1 },		{ 0.2f,  1.0f	},		{ 0.3f, 0.5f, 0.2f, 0.2f }	},		// noise
	{	{ 0.6f, 0.5f, 0 },		{ 0.f, 0.f, 0 },		{ 0.25f, 1.0f	},		{}				},					// bass
	{	{ 0.4f, 0.3f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 0.3f, 0.5f, 0.2f, 0.2f }	},		// pad  
	{	{ 0.4f, 0.3f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 0.3f, 0.5f, 0.2f, 0.2f }	},		// second pad  
	{	{ 0.4f, 0.5f, 0 },		{},						{ 0.15f, 1.0f	},		{} }								// kick
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

	//Clipper.Process(pBuffers, Frames);
}
 