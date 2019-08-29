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
		"Hello there. We are MZPE team, and this is our first demo. Please, enjoy!\n\nTo continue, press 'OK' button.",
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

