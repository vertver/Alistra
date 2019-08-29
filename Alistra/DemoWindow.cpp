/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "DemoSettings.h"

bool
OpenSettings(
	bool& bVSYNC, 
	bool& bFullscreen
)
{
	if (MessageBoxA(
		nullptr,
		"Hello there. We are MZPE team, and this is our first demonstration on compute shaders. Press 'OK' to continue",
		"Information",
		MB_OKCANCEL | MB_ICONINFORMATION) == IDOK 
	)
	{
		if (MessageBoxA(nullptr, "Enable V-Sync?", "Information", MB_YESNO | MB_ICONINFORMATION) == IDYES)
		{
			bVSYNC = true;
		}

		return true;
	}

	return false;
}

