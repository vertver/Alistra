#include "DemoSettings.h"

bool
OpenSettings(
	bool& bVSYNC, 
	bool& bFullscreen
)
{
	if (MessageBoxA(
		nullptr,
		"This is Alistra demo build special for Chaos Construction 2019. Please, select vsync option after this message box. If you want to close application - select 'cancel button'",
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

