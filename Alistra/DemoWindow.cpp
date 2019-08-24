#include "DemoSettings.h"

bool
OpenSettings(
	bool& bVSYNC, 
	bool& bFullscreen
)
{
	if (MessageBoxA(
		nullptr,
		"This is Alistra demo build special for Chaos Construction 2019. Please, select V-Sync option after this message box. If you want to close application - select 'Cancel' button",
		"»Õ‘Œ–Ã¿“»ŒÕ",
		MB_OKCANCEL | MB_ICONINFORMATION) == IDOK 
	)
	{
		if (MessageBoxA(nullptr, "Enable V-Sync?", "»Õ‘Œ–Ã¿“»ŒÕ", MB_YESNO | MB_ICONINFORMATION) == IDYES)
		{
			bVSYNC = true;
		}

		return true;
	}

	return false;
}

