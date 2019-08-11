#pragma once
#include "Base_Sound.h"
#include <math.h>

#define MAX_SYNTHS 24
#define MAX_POLY 6
#define SYNTHBUFFER_SIZE 1024

enum ESTAGE 
{
	ESTAGE_ATTACK,
	ESTAGE_DECAY,
	ESTAGE_SUSTAIN,
	ESTAGE_RELEASE,
	ESTAGE_OFF
};

class CADSREnvelope
{
private:
	f32 AttackBase;
	f32 AttackKoef;
	f32 DecayBase;
	f32 DecayKoef;
	f32 ReleaseBase;
	f32 ReleaseKoef;
	f32 SustainLevel;
	ADSR_STRUCT ADSRStruct;

public:
	void Initialize(ADSR_STRUCT* pADSRStruct, f32 SampleRate);
	void Reset();

	f32 NextEnvelope();
};
