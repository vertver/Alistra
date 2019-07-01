#include "Base.h"
#include "Base_Sound.h"
#include <stdio.h>

int
RealEntryPoint(
	char** argv,
	int argc
)
{
	boolean isImgui = 0;

	for (size_t i = 0; i < argc; i++)
	{
		/*
			Ignore path to file. We don't need to process it
		*/
		if (!i) continue;

		if (strcmp(argv[i], "-use_imgui"))
		{
			isImgui = true;
			break;
		}
	}

  	if (!OpenGL_Library()) 
  	{ 
  		return -1; 
  	}
  	
  	int Funcs = 0;
 
	load_procs();

	SOUNDDEVICE_INFO** pInfo = NULL;
	size_t CountDevices = 0;

	if (!EnumerateSoundDevices(&pInfo, &CountDevices))
	{
		return -1;
	}

	/*
		Test zone
	*/
	{
		char szId[512];
		memset(szId, 0, sizeof(szId));

		for (size_t i = 0; i < CountDevices; i++)
		{
			sprintf_s(szId, 512, "%u - (%s)", pInfo[i]->Fmt.Index, pInfo[i]->szName);
			MessageBoxA(NULL, pInfo[i]->szGUID, szId, MB_OK);
			memset(szId, 0, sizeof(szId));
		}
	}

	return Funcs;
}