#pragma once
#include "Base_Sound.h"

typedef struct  
{
	size_t FramesStart;		// Kick start position, in frames
	size_t FrameCount;		// Kick length, in frames
} KICK_POSES;

typedef struct  
{
	f32 KickBodyFrequency;
	f32 KickBodyMilliseconds;
	f32 KickBodyLength;
	f32 KickMainMilliseconds;
	f32 KickMainLength;
} KICK_STRUCT;

class CKickGenerator
{
	f32 SampleRate;
	f32 fFrequencyCoef;
	f32 fKickPhase;
	f32 fKickFrequency;
	f32 fFirstBody;
	f32 fSecondBody;
	size_t CountOfStructs;
	size_t CurrentKick;
	KICK_POSES** ppKickStruct;
	KICK_STRUCT KickSettings;

public:
	void Initialize(f32 fSampleRate, KICK_POSES* pKickStruct, KICK_STRUCT* pKickSettings, size_t StructsCount);
	void Destroy();

	void Process(f32** pBuffers, size_t Frames);
};
