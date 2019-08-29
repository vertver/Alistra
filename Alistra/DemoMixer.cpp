/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "DemoMixer.h"

const EFFECT_STRUCT AllEffectsForSynths[MAX_SYNTHS] =
{
	{	{ 0.1f, 0.35f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 270.f,	0.5f,	0.08f }		},		// main arp
	{	{ 0.1f, 0.35f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 280.f,	0.8f,	0.12f }		},		// second arp
	{	{ 0.2f, 0.5f, 0 },		{ 0.5f, 0.08f, 1 },		{ 0.2f,  1.0f	},		{ 500.f,	0.75f,	0.15f }		},		// noise
	{	{ 0.2f, 0.6f, 0 },		{ 0.2f, 0.f, 0 },		{ 0.10f, 1.0f	},		{ 0.f,		0.f,	0.f	  }		},		// bass
	{	{ 0.2f, 0.3f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 320.f,	0.7f,	0.15f }		},		// pad  
	{	{ 0.2f, 0.3f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.0f,  1.0f	},		{ 350.f,	0.7f,	0.15f }		},		// second pad  
	{	{ 0.2f, 0.5f, 0 },		{ 0.f, 0.0f, 0 },		{ 0.15f, 1.0f	},		{ 0.f,		0.f,	0.f }		},		// kick
	{	{ 0.1f, 0.35f, 0 },		{ 0.f, 0.08f, 1 },		{ 0.0f,  1.0f	},		{ 380.f,	0.8f,	0.12f }		},		// first main noise
	{	{ 0.1f, 0.35f, 0 },		{ 0.f, 0.08f, 1 },		{ 0.0f,  1.0f	},		{ 320.f,	0.8f,	0.12f }		}		// second main noise
};

const PANNER_STRUCT AllPannerSettings[MAX_SYNTHS] =
{
	{ 0.45f, 1.0f },
	{ 0.55f, 0.92f },
	{ 0.5f, 0.75f },
	{ 0.5f, 0.50f },
	{ 0.58f, 0.15f },
	{ 0.42f, 0.15f },
	{ 0.5f, 1.0f },
	{ 0.5f, 0.5f },
	{ 0.5f, 0.5f }
};

void
CDemoMixer::Initialize(WAVE_FMT InFmt, AUTOMATION_STRUCT** pStruct, size_t* Automations)
{
	CLIPPER_STRUCT ThisStruct = {};
	ThisStruct.fDrive = 0.f;
	ThisStruct.fMix = 1.f;

	NoteManager.Initialize(InFmt);
	Clipper.Initialize(&ThisStruct);

	for (size_t i = 0; i < MAX_SYNTHS; i++)
	{
		Effects[i].Initalize((EFFECT_STRUCT*)&AllEffectsForSynths[i], (f32)InFmt.SampleRate);
		Panner[i].Initialize((PANNER_STRUCT*)&AllPannerSettings[i]);

// 		if (pStruct[i] || Automations[i])
// 		{
// 			pAutomation[i] = new CFilterAutomation;
// 			pAutomation[i]->Initialize(pStruct[i], Automations[i], (FILTER_STRUCT*)&AllEffectsForSynths[i].FilterSettings1, 0);
// 		}
	}
}

void
CDemoMixer::Destroy()
{
	NoteManager.Destroy();
	for (size_t i = 0; i < MAX_SYNTHS; i++)
	{
		if (pAutomation[i])
		{
			pAutomation[i]->Destroy();
			delete pAutomation[i];
		}
	}
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
// 
// 		if (pAutomation[i])
// 		{
// 			for (size_t o = 0; o < Frames; o++)
// 			{
// 				if (pAutomation[i]->NextSample())
// 				{
// 					FILTER_STRUCT TempStruct = {};
// 					pAutomation[i]->GetFiter(&TempStruct);
// 					Effects[i].SetFilterOptions(&TempStruct);
// 				}
// 			}
// 		}

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
 