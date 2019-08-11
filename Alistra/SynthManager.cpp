#include "SynthManager.h"

const SYNTH_STRUCT SynthGlobalStruct[MAX_SYNTHS][MAX_POLY];

void
CSynthManager::Initialize(WAVE_FMT InFmt)
{
	memcpy(&MixFmt, &InFmt, sizeof(WAVE_FMT));

	Voices = (CBaseVoiceClass***)HeapAlloc(GetProcessHeap(), 0, sizeof(CBaseVoiceClass***) * MAX_SYNTHS);

	for (size_t i = 0; i < MAX_SYNTHS; i++)
	{
		Voices[i] = (CBaseVoiceClass**)HeapAlloc(GetProcessHeap(), 0, sizeof(CBaseVoiceClass**) * MAX_POLY);

		for (size_t j = 0; j < MAX_POLY; j++)
		{
			Voices[i][j] = new CBaseVoiceClass;
			Voices[i][j]->Initialize((SYNTH_STRUCT*)&SynthGlobalStruct[i][j], MixFmt.SampleRate);
		}
	}

	NoteManager.Initialize(MixFmt);
}

void
CSynthManager::Destroy()
{
	for (size_t i = 0; i < MAX_SYNTHS; i++)
	{
		for (size_t j = 0; j < MAX_POLY; j++)
		{
			if (Voices[i][j]) delete Voices[i][j];
		}

		if (Voices[i]) HeapFree(GetProcessHeap(), 0, Voices[i]);
	}

	if (Voices) HeapFree(GetProcessHeap(), 0, Voices);

	NoteManager.Destroy();
}

void
CSynthManager::Process(float** pTemp, float** pBuffers, size_t Frames) 
{
	/*
		Process every synth
	*/
	for (size_t i = 0; i < MAX_SYNTHS; i++)
	{
		i32 ThisNote = 0;
		i32 ThisVel = 0;
		f32 ThisVolume = 0.f;
		f32* pDataToTemp[2] = {};
		size_t FramesThis = 0;
		size_t FramesToEnd[MAX_POLY] = {};

		pDataToTemp[0] = pTemp[0];
		pDataToTemp[1] = pTemp[1];

		for (size_t j = 0; j < MAX_POLY; j++)
		{
			size_t FramesT = NotesToUpdate[i][j].NoteFrames;
			CBaseVoiceClass* pCurrentVoice = Voices[i][j];

			/*
				Process last frames in voice
			*/
			if (FramesT && FramesT < Frames)
			{
				for (size_t o = 0; o < FramesT; o++)
				{
					f32 fTemp = pCurrentVoice->NextSample();
					pDataToTemp[0][o] = fTemp;
					pDataToTemp[1][o] = fTemp;
				}

				pDataToTemp[0] = &pDataToTemp[0][FramesT];
				pDataToTemp[1] = &pDataToTemp[1][FramesT];

				FramesT = 0;
			}

			/*
				If we didn't have any sample - try to check new for our voice
			*/
			if (!FramesT)
			{	
				FramesT = Frames - NotesToUpdate[i][j].NoteFrames;
				NoteManager.GetCurrentVoiceState(i, j, NotesToUpdate[i][j]);
				pCurrentVoice->NoteOn(NotesToUpdate[i][j].NoteIndex, NotesToUpdate[i][j].NoteVelocity);
			}

			/*
				Process last samples
			*/
			for (size_t o = 0; o < FramesT; o++)
			{
				f32 fTemp = pCurrentVoice->NextSample();
				pDataToTemp[0][o] = fTemp;
				pDataToTemp[1][o] = fTemp;
			}

			NotesToUpdate[i][j].NoteFrames -= FramesT;
		}

		EffectsClass.Process(pTemp, Frames);

		for (size_t o = 0; o < Frames; o++)
		{
			pBuffers[0][o] += pTemp[0][o];
			pBuffers[1][o] += pTemp[1][o];
		}
	}
}
