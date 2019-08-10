#include "KickGenerator.h"

f32 KickFreq1 = 1000;
f32 KickFreq2 = 158 * 2.0;
f32 KickFreq3 = 58;

void
CKickGenerator::Initialize(f32 fSampleRate, KICK_POSES* pKickStruct, KICK_STRUCT* pKickSettings, size_t StructsCount)
{
	ppKickStruct = (KICK_POSES**)HeapAlloc(GetProcessHeap(), 0, sizeof(KICK_POSES*) * (StructsCount + 1));

	for (size_t i = 0; i < StructsCount; i++)
	{
		ppKickStruct[i] = (KICK_POSES*)HeapAlloc(GetProcessHeap(), 0, sizeof(KICK_POSES));
	}

	memcpy(&KickSettings, pKickSettings, sizeof(KICK_STRUCT));

	SampleRate = fSampleRate;
	CountOfStructs = StructsCount;
}

void
CKickGenerator::Destroy()
{
	for (size_t i = 0; i < CountOfStructs; i++)
	{
		if (ppKickStruct[i]) HeapFree(GetProcessHeap(), 0, ppKickStruct[i]);
	}

	if (ppKickStruct) HeapFree(GetProcessHeap(), 0, ppKickStruct);
}

void
CKickGenerator::Process(f32** pBuffers, size_t Frames)
{
	if (CurrentKick < CountOfStructs)
	{
		/*
		  for ( u32 N = 0; N < Kick_Len; N++ ) {

		f32 Out     = sin(Kick_Faza);

		Kick_Faza += FreqKoef * Kick_Freq;

		Kick_Freq += (Kick_Freq_3 - Kick_Freq)*0.001*2.0;

	   // Out     += Body2*((f32)rand()/(f32)RAND_MAX);

		Out = min(1.0, (sin(Body*3.1415*0.5) * Out));

		if (Out > 0.5) Out = (Out - 0.5)*0.5 + 0.5;

		Kick_DataMono[N] = 0.9 * Out;

		Body  += (0 - Body)*(0.00009)*2.0*0.8;
		Body2 += (0 - Body2)*(0.0017);
	}
		*/
	}
}
