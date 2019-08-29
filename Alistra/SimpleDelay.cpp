/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "Base_Sound.h"

void
CDelay::Initialize(u32 Length, f32 Volume)
{
	Reset();
	pDelayBuffer = (f32*)HeapAlloc(GetProcessHeap(), 0, Length * sizeof(f32));
	memset(pDelayBuffer, 0, Length * sizeof(f32));
	DelayLength = Length;
	fVolume = Volume;
}

void
CDelay::Reset()
{
	if (pDelayBuffer) HeapFree(GetProcessHeap(), 0, pDelayBuffer);
	DelayLength = 0;
	Position = 0;
}

f32
CDelay::Fetch()
{
	return pDelayBuffer[Position] * fVolume;
}

void
CDelay::Feed(f32 Value)
{
	pDelayBuffer[Position] = Value;
	Position++;
	if (Position >= DelayLength) Position = 0;
}
