/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "ADSR_Synthesizer.h"

void
CADSREnvelope::Initialize(ADSR_STRUCT* pADSRStruct, f32 SampleRate)
{
	UpdateADSR(pADSRStruct, SampleRate);
}

void
CADSREnvelope::Reset()
{
	ADSRStruct.fEnvelopeLevel = 0.0;
	ADSRStruct.iEnvelopeStage = ESTAGE_OFF;
}

void
CADSREnvelope::UpdateADSR(ADSR_STRUCT* pADSRStruct, f32 SampleRate)
{
	memcpy(&ADSRStruct, pADSRStruct, sizeof(ADSR_STRUCT));
	f32 A = pADSRStruct->fAttack * 0.001f * SampleRate;
	f32 D = pADSRStruct->fDecay * 0.001f * SampleRate;
	f32 R = pADSRStruct->fRelease * 0.001f * SampleRate;

	SustainLevel = pADSRStruct->fSustain;
	AttackKoef = (A <= 0.0f) ? 0.0f : expf(-logf((1.0f + pADSRStruct->fAttackCurve) / pADSRStruct->fAttackCurve) / A);
	DecayKoef = (D <= 0.0f) ? 0.0f : expf(-logf((1.0f + pADSRStruct->fDecayReleaseCurve) / pADSRStruct->fDecayReleaseCurve) / D);
	ReleaseKoef = (R <= 0.0f) ? 0.0f : expf(-logf((1.0f + pADSRStruct->fDecayReleaseCurve) / pADSRStruct->fDecayReleaseCurve) / R);
	AttackBase = (1.0f + pADSRStruct->fAttackCurve) * (1.0f - AttackKoef);
	DecayBase = (SustainLevel - pADSRStruct->fDecayReleaseCurve) * (1.0f - DecayKoef);
	ReleaseBase = -pADSRStruct->fDecayReleaseCurve * (1.0f - ReleaseKoef); 
}

void
CADSREnvelope::BeginAttack()
{
	ADSRStruct.iEnvelopeStage = ESTAGE_ATTACK;
}

void
CADSREnvelope::BeginRelease()
{
	ADSRStruct.iEnvelopeStage = ESTAGE_RELEASE;
}

i32
CADSREnvelope::GetCurrentState()
{
	return ADSRStruct.iEnvelopeStage;
}

f32
CADSREnvelope::NextEnvelope()
{
	switch (ADSRStruct.iEnvelopeStage)
	{
	case ESTAGE_ATTACK:
		ADSRStruct.fEnvelopeLevel = ADSRStruct.fEnvelopeLevel * AttackKoef + AttackBase;
		if (ADSRStruct.fEnvelopeLevel >= 1.0)
		{
			ADSRStruct.fEnvelopeLevel = 1.0;
			ADSRStruct.iEnvelopeStage = ESTAGE_DECAY;
		}
		break;

	case ESTAGE_DECAY:
		ADSRStruct.fEnvelopeLevel = ADSRStruct.fEnvelopeLevel * DecayKoef + DecayBase;
		if (ADSRStruct.fEnvelopeLevel <= SustainLevel)
		{
			ADSRStruct.fEnvelopeLevel = SustainLevel;
			ADSRStruct.iEnvelopeStage = ESTAGE_SUSTAIN;
		}
		break;

	case ESTAGE_SUSTAIN:
		break;

	case ESTAGE_RELEASE:
		ADSRStruct.fEnvelopeLevel = ADSRStruct.fEnvelopeLevel* ReleaseKoef + ReleaseBase;
		if (ADSRStruct.fEnvelopeLevel <= 0.0)
		{
			ADSRStruct.fEnvelopeLevel = 0.0;
			ADSRStruct.iEnvelopeStage = ESTAGE_OFF;
		}
		break;

	default: 
		ADSRStruct.fEnvelopeLevel = 0.0;
		break;
	}
	
	return ADSRStruct.fEnvelopeLevel;
}
