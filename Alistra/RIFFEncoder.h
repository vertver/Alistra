/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once
#include "Base_Sound.h"

class CRiffEncoder
{
private:
	size_t HeaderSize;
	HANDLE hFile;
	WAVE_FMT FileFmt;

public:
	CRiffEncoder() : HeaderSize(0), hFile(0) { memset(&FileFmt, 0, sizeof(WAVE_FMT)); }
	bool Initialize(const wchar_t* szPathToFile, WAVE_FMT FileFormat);
	void Write(void* pData, size_t SizeInBytes);
	void Destroy();
};
