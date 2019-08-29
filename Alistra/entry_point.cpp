#include "Base.h"
#include "Base_Sound.h"

/*
	kkrunchy compressor doesn't support exports
*/
#ifdef DEBUG
_declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
#endif

int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nShowCmd
)
{
	size_t HeapInfo = 2;
	LPSTR* lpOutArgList = NULL;
	LPWSTR* lpArgList = NULL;
	int iCount = 0;
	int iRet = 0;

#ifdef DEBUG
	HeapSetInformation(GetProcessHeap(), HeapCompatibilityInformation, &HeapInfo, sizeof(size_t));

	/*
		Get 'argv' and 'argc' in widechar to convert
		this string to UTF-8
	*/
	lpArgList = CommandLineToArgvW(GetCommandLineW(), &iCount);
	lpOutArgList = (LPSTR*) HeapAlloc(GetProcessHeap(), 0, sizeof(void*) * iCount);

	/*
		Parse command line and convert to UTF-8
	*/
	for (int i = 0; i < iCount; i++)
	{
		size_t sizeOfString = 0;

		if (!!(sizeOfString = wcslen(lpArgList[i])))
		{
			// get size of new UTF-8 string
			int StringSize = WideCharToMultiByte(CP_UTF8, 0, lpArgList[i], -1, NULL, 0, NULL, NULL);

			if (StringSize)
			{
				// allocate string
				lpOutArgList[i] = (LPSTR) HeapAlloc(GetProcessHeap(), 0, ++StringSize);

				// convert to UTF-8
				if (!WideCharToMultiByte(CP_UTF8, 0, lpArgList[i], -1, lpOutArgList[i], StringSize, NULL, NULL))
				{
					HeapFree(GetProcessHeap(), 0, lpOutArgList[i]);
					lpOutArgList[i] = NULL;
					continue;
				}
			}
		}
	}
#endif

	/*
		Init COM stuff
	*/
	CoInitialize(NULL);

	iRet = RealEntryPoint(lpOutArgList, iCount);

#ifdef DEBUG
	/*
		Free all stuff
	*/
	for (int i = 0; i < iCount; i++)
	{
		if (lpOutArgList[i]) HeapFree(GetProcessHeap(), 0, lpOutArgList[i]);
	}

	HeapFree(GetProcessHeap(), 0, lpOutArgList);
	LocalFree(lpArgList);
#endif
	CoUninitialize();

	return iRet;
}
