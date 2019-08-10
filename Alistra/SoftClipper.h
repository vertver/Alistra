#pragma once
#include "Base_Sound.h"

typedef struct  
{
	f32 fDrive;
	f32 fMix;
} CLIPPER_STRUCT;

class CSoftClipper
{
private:
	CLIPPER_STRUCT ClipperStruct;

public:
	void Initialize(CLIPPER_STRUCT* pClipperStruct);
	void Reset();

	void Process(f32** pBuffers, size_t Frames);
};
