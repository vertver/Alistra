#include "Base.h"
#include <versionhelpers.h>
#pragma comment(lib, "Version.lib")

boolean
GetSystemVersion(BASE_OS_VERSION_INFO* pVersion)
{
	wchar_t szPath[260];
	wchar_t StringInfo[512];

	if (IsWindows7OrGreater())
	{
		pVersion->MajorVersion = 6;
		pVersion->MinorVersion = 1;
	}
	else
	{
		MessageBoxA(NULL, "Sorry, but systems below Windows 7 are not supported by this demo.", "ЕГГОГ", MB_OK | MB_ICONERROR);
		return false;
	}

	if (IsWindows8OrGreater())
	{
		pVersion->MajorVersion = 6;
		pVersion->MinorVersion = 2;
	}

	// it's not just working before 8.1, because assholes in Microsoft can't 
	// make basic function for check Windows version
	if (IsWindows8Point1OrGreater())
	{
		pVersion->MajorVersion = 6;
		pVersion->MinorVersion = 3;
	}

	if (IsWindows10OrGreater())
	{
		pVersion->MajorVersion = 10;
		pVersion->MinorVersion = 0;
	}

	// get system directory for check dll version
	if (GetSystemDirectoryW(szPath, sizeof(szPath)))
	{
		// use more updated library from system directory
		wcscat_s(szPath, ARRAYSIZE(szPath), L"\\shell32.dll");

		VS_FIXEDFILEINFO* pvi;
		DWORD sz = sizeof(VS_FIXEDFILEINFO);

		// get Windows version from ntdll library
		if (GetFileVersionInfoW(szPath, 0, ARRAYSIZE(StringInfo), StringInfo))
		{
			if (VerQueryValueW(&StringInfo[0], L"\\", (LPVOID*)&pvi, (unsigned int*)&sz))
			{
				pVersion->MajorVersion = HIWORD(pvi->dwProductVersionMS);
				pVersion->MinorVersion = LOWORD(pvi->dwProductVersionMS);
				pVersion->BuildVersion = HIWORD(pvi->dwProductVersionLS);
				pVersion->MarketVersion = LOWORD(pvi->dwProductVersionLS);
			}
		}

		return true;
	}

	return false;
}

