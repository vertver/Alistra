/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once
#include "Base_Sound.h"

class CADSRAutomation
{
private:
	f32 fToAdd;
	size_t CurrentSample;
	size_t CurrentAutomation;
	size_t ThisAutomationCount;
	size_t ADSRType;
	AUTOMATION_STRUCT** Automation;
	ADSR_STRUCT InternalADSR;

public:
	void Initialize(AUTOMATION_STRUCT** pAutomation, size_t AutomationCount, ADSR_STRUCT* pADSR, size_t ParamToManipulate)
	{
		memcpy(&InternalADSR, pADSR, sizeof(ADSR_STRUCT));

		ADSRType = ParamToManipulate;
		ThisAutomationCount = AutomationCount + 1;
		Automation = (AUTOMATION_STRUCT**)HeapAlloc(GetProcessHeap(), 0, sizeof(AUTOMATION_STRUCT*) * ThisAutomationCount);

		for (size_t i = 0; i < AutomationCount; i++)
		{
			Automation[i] = (AUTOMATION_STRUCT*)HeapAlloc(GetProcessHeap(), 0, sizeof(AUTOMATION_STRUCT));
			memcpy(Automation[i], pAutomation[i], sizeof(AUTOMATION_STRUCT));
		}

		size_t TempValue = (size_t)(Automation[1]->Frame - Automation[0]->Frame);
		fToAdd = Automation[1]->fPoint - Automation[0]->fPoint;
		fToAdd /= (f32)TempValue;
	}

	void Destroy()
	{
		for (size_t i = 0; i < ThisAutomationCount; i++)
		{
			if (Automation[i]) HeapFree(GetProcessHeap(), 0, Automation[i]);
		}

		if (Automation) HeapFree(GetProcessHeap(), 0, Automation);
	}

	bool NextSample()
	{
		bool bRet = false;
		if (Automation)
		{
			if (CurrentSample == Automation[CurrentAutomation]->Frame)
			{
				size_t TempValue = 0;
				i64 Temp1 = (i64)Automation[CurrentAutomation + 1]->Frame;
				i64 Temp2 = (i64)Automation[CurrentAutomation]->Frame;
				CurrentAutomation = CurrentAutomation < ThisAutomationCount ? CurrentAutomation : ThisAutomationCount - 2;
				TempValue = abs(Temp1 - Temp2);
				fToAdd = Automation[CurrentAutomation]->fPoint - Automation[CurrentAutomation + 1]->fPoint;
				fToAdd /= (f32)TempValue;
			}

			if (CurrentSample % 30)
			{
				bRet = true;
			}

			switch (ADSRType)
			{
			case 0:
				InternalADSR.fAttack += fToAdd;
				break;
			case 1:
				InternalADSR.fDecay += fToAdd;
				break;
			case 2:
				InternalADSR.fRelease += fToAdd;
				break;
			default:
				break;
			}

			CurrentSample++;
		}

		return bRet;
	}
	void GetADSR(ADSR_STRUCT* pADSR)
	{
		memcpy(pADSR, &InternalADSR, sizeof(ADSR_STRUCT));
	}
};

class CFilterAutomation
{
private:
	f32 fToAdd;
	size_t CurrentSample;
	size_t ToManipulate;
	size_t CurrentAutomation;
	size_t ThisAutomationCount;
	AUTOMATION_STRUCT** Automation;
	FILTER_STRUCT InternalFilter;

public:
	CFilterAutomation() : fToAdd(0.f), CurrentSample(0), ToManipulate(0), CurrentAutomation(0), ThisAutomationCount(0)
	{

	}

	void Initialize(AUTOMATION_STRUCT* pAutomation, size_t AutomationCount, FILTER_STRUCT* pADSR, size_t ParamToManipulate)
	{
		memcpy(&InternalFilter, pADSR, sizeof(FILTER_STRUCT));

		ToManipulate = ParamToManipulate;
		ThisAutomationCount = AutomationCount + 1;
		Automation = (AUTOMATION_STRUCT**)HeapAlloc(GetProcessHeap(), 0, sizeof(AUTOMATION_STRUCT*) * ThisAutomationCount);

		for (size_t i = 0; i < AutomationCount; i++)
		{
			Automation[i] = (AUTOMATION_STRUCT*)HeapAlloc(GetProcessHeap(), 0, sizeof(AUTOMATION_STRUCT));
			memcpy(Automation[i], &pAutomation[i], sizeof(AUTOMATION_STRUCT));
		}

		size_t TempValue = (size_t)(Automation[1]->Frame - Automation[0]->Frame);
		fToAdd = Automation[1]->fPoint - Automation[0]->fPoint;
		fToAdd /= (f32)TempValue;
	}

	void Destroy()
	{
		for (size_t i = 0; i < ThisAutomationCount; i++)
		{
			if (Automation[i]) HeapFree(GetProcessHeap(), 0, Automation[i]);
		}

		if (Automation) HeapFree(GetProcessHeap(), 0, Automation);
	}

	bool NextSample()
	{
		bool bRet = false;
		if (Automation)
		{
			if (CurrentSample == Automation[CurrentAutomation]->Frame)
			{
				size_t TempValue = 0;
				i64 Temp1 = (i64)Automation[CurrentAutomation + 1]->Frame;
				i64 Temp2 = (i64)Automation[CurrentAutomation]->Frame;
				CurrentAutomation = CurrentAutomation < ThisAutomationCount ? CurrentAutomation : ThisAutomationCount - 2;
				TempValue = abs(Temp1 - Temp2);
				fToAdd = Automation[CurrentAutomation]->fPoint - Automation[CurrentAutomation + 1]->fPoint;
				fToAdd /= (f32)TempValue;
			}

			if (CurrentSample % 30)
			{
				bRet = true;
			}

			switch (ToManipulate)
			{
			case 0:
				InternalFilter.fFrequency += fToAdd;
				break;
			case 1:
				InternalFilter.fResonance += fToAdd;
				break;
			default:
				break;
			}

			CurrentSample++;
		}

		return bRet;
	}

	void GetFiter(FILTER_STRUCT* pFilter)
	{
		memcpy(pFilter, &InternalFilter, sizeof(FILTER_STRUCT));
	}
};
