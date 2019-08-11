#pragma once
#include "Base_Sound.h"

class CEffectsClass
{
public:
	void Initalize();
	void Destroy();

	void Process(float** pBuffers, size_t Frames);
};
