/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "RIFFEncoder.h"

bool
CRiffEncoder::Initialize(const wchar_t* szPathToFile, WAVE_FMT FileFormat)
{
	WORD wTemp = 0;
	DWORD dwTempSize = 0;
	DWORD dwTemp = 0;
	char TempArray[128] = {};

	/*
		PCM Wave can't be with sample rate lower than 8000
	*/
	memcpy(&FileFmt, &FileFormat, sizeof(WAVE_FMT));
	if (FileFmt.SampleRate < 8000 || FileFmt.Bits < 8 || !FileFmt.Channels)
	{
		Destroy();
		return false;
	}

	/*
		Try to create wave file
	*/
	hFile = CreateFileW(szPathToFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE || !hFile) return false;

	return true;
}

void
CRiffEncoder::Destroy()
{
	if (hFile == INVALID_HANDLE_VALUE || !hFile) return;
	CloseHandle(hFile);
}

void
CRiffEncoder::Write(void* pData, size_t SizeInBytes)
{
	if (hFile || hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWriteDataSize = 0;
		DWORD dwTempSize = 0;
		DWORD dwTemp2 = 0;
		DWORD dwTemp = 0;
		char* pTempPointer = (char*)&dwTemp2;
		char cbTemp[8] = {};
		
		/*
			Put 'data' chunk signature and full data
		*/
		cbTemp[0] = 'd';
		cbTemp[1] = 'a';
		cbTemp[2] = 't';
		cbTemp[3] = 'a';
		dwTemp = (DWORD)SizeInBytes;
		dwTemp2 = dwTemp;
		cbTemp[4] = pTempPointer[0];
		cbTemp[5] = pTempPointer[1];
		cbTemp[6] = pTempPointer[2];
		cbTemp[7] = pTempPointer[3];

		WriteFile(hFile, pData, (DWORD)SizeInBytes, &dwWriteDataSize, nullptr);
		FlushFileBuffers(hFile);
	}
}
