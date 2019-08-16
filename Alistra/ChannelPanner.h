#pragma once
#include "Base_Sound.h"

typedef struct
{
	f32 fPan;
	f32 fVolume;
} PANNER_STRUCT;

class CChannelPanner
{
private:
	PANNER_STRUCT PannerStruct;

public:
	void Initialize(PANNER_STRUCT* pPannerStruct);
	void Process(f32** pBuffers, size_t Frames);
	void Reset();
};
