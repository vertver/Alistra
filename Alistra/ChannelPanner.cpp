#include "ChannelPanner.h"

void
CChannelPanner::Initialize(PANNER_STRUCT* pPannerStruct)
{
	memcpy(&PannerStruct, pPannerStruct, sizeof(PANNER_STRUCT));
}

void
CChannelPanner::Reset()
{
	memset(&PannerStruct, 0, sizeof(PANNER_STRUCT));
}

void
CChannelPanner::Process(f32** pBuffers, size_t Frames)
{
	f32 fPan = PannerStruct.fPan;
	f32 fVolume = PannerStruct.fVolume;	
	f32 fLeftPan = fabsf(fPan - 1.f);
	f32 fRightPan = fPan;

	for (size_t i = 0; i < Frames; i++)
	{
		pBuffers[0][i] *= fLeftPan * fVolume * 2.f;
		pBuffers[1][i] *= fRightPan * fVolume * 2.f;
	}
}
