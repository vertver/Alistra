#pragma once
#include "Base_Sound.h"

class CNoteManager
{
public:
	void Initialize();
	void Destroy();

	void GetNextNote(i32 Synth, i32& NextNote, i32& Velocity, f32& fVolume, size_t& Frames)
};

