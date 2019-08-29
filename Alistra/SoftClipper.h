/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once
#include "Base_Sound.h"

class CSoftClipper
{
private:
	CLIPPER_STRUCT ClipperStruct;

public:
	void Initialize(CLIPPER_STRUCT* pClipperStruct);
	void Reset();

	void Process(f32** pBuffers, size_t Frames);
};
