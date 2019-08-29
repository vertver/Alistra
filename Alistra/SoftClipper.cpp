/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "SoftClipper.h"

void
CSoftClipper::Initialize(CLIPPER_STRUCT* pClipperStruct)
{
	memcpy(&ClipperStruct, pClipperStruct, sizeof(CLIPPER_STRUCT));
}

void
CSoftClipper::Reset()
{
	ClipperStruct.fDrive = 0.f;
	ClipperStruct.fMix = 0.f;
}

void
CSoftClipper::Process(f32** pBuffers, size_t Frames)
{
	f32 Temp = 0.f;
	f32 DriveLevel = ClipperStruct.fDrive;
	f32 Mix = ClipperStruct.fMix;

	if (DriveLevel > 0.f)
	{
		for (size_t o = 0; o < 2; o++)
		{
			float* OutCh = pBuffers[o];

			for (size_t i = 0; i < Frames; i++) {
				Temp = OutCh[i];
				Temp += OutCh[i] * DriveLevel * 3.141592653f;
				Temp = signf(Temp) * powf(fastatanf(powf(fabsf(Temp), 25.f)), (1 / 25.f));
				OutCh[i] = (Temp * Mix) + (fabsf(Mix - 1.0f) * OutCh[i]);
			}
		}
	}
}
