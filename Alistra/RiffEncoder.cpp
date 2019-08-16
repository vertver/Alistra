#include "RIFFEncoder.h"

inline
bool
SetSoundFileFormat(void* pData, size_t FileDataSize, WAVE_FMT& waveFormat)
{
	if (FileDataSize < 36) return false;
	char* pByteData = (char*)pData;
	char* pTempByteData1 = nullptr;
	char* pTempByteData2 = nullptr;
	WORD wTemp = 0;
	WORD wTemp2 = 0;
	DWORD dwTemp = 0;
	DWORD dwTemp2 = 0;

	pTempByteData1 = (char*)&dwTemp;
	pTempByteData2 = (char*)&wTemp;

	pByteData[0] = 'R';
	pByteData[1] = 'I';
	pByteData[2] = 'F';
	pByteData[3] = 'F';

	dwTemp2 = 4;
	dwTemp = dwTemp2;
	pByteData[4] = pTempByteData1[0];
	pByteData[5] = pTempByteData1[1];
	pByteData[6] = pTempByteData1[2];
	pByteData[7] = pTempByteData1[3];

	pByteData[8] = 'W';
	pByteData[9] = 'A';
	pByteData[10] = 'V';
	pByteData[11] = 'E';

	pByteData[12] = 'f';
	pByteData[13] = 'm';
	pByteData[14] = 't';
	pByteData[15] = ' ';

	dwTemp2 = 16;
	dwTemp = dwTemp2;
	pByteData[16] = pTempByteData1[0];
	pByteData[17] = pTempByteData1[1];
	pByteData[18] = pTempByteData1[2];
	pByteData[19] = pTempByteData1[3];

	wTemp2 = waveFormat.IsFloat ? 3 : 1;
	dwTemp = wTemp2;
	pByteData[20] = pTempByteData2[0];
	pByteData[21] = pTempByteData2[1];

	wTemp2 = waveFormat.Channels;
	if (!wTemp2) return false;
	wTemp = wTemp2;
	pByteData[22] = pTempByteData2[0];
	pByteData[23] = pTempByteData2[1];

	dwTemp2 = waveFormat.SampleRate;
	if (dwTemp2 < 8000) return false;
	dwTemp = dwTemp2;
	pByteData[24] = pTempByteData1[0];
	pByteData[25] = pTempByteData1[1];
	pByteData[26] = pTempByteData1[2];
	pByteData[27] = pTempByteData1[3];

	dwTemp2 = waveFormat.SampleRate * waveFormat.Channels * (waveFormat.Bits / 8);
	dwTemp = dwTemp2;
	pByteData[28] = pTempByteData1[0];
	pByteData[29] = pTempByteData1[1];
	pByteData[30] = pTempByteData1[2];
	pByteData[31] = pTempByteData1[3];

	wTemp2 = waveFormat.Channels* (waveFormat.Bits / 8);
	wTemp = wTemp2;
	pByteData[32] = pTempByteData2[0];
	pByteData[33] = pTempByteData2[1];

	wTemp2 = waveFormat.Bits;
	if (wTemp2 < 8) return false;
	wTemp = wTemp2;
	pByteData[34] = pTempByteData2[0];
	pByteData[35] = pTempByteData2[1];
	return true;
}

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

	if (!SetSoundFileFormat(TempArray, 128, FileFmt))
	{
		Destroy();
		return false;
	}

	/*
		Try to create wave file
	*/
	hFile = CreateFileW(szPathToFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE || !hFile) return false;

	/*
		Write temp array data to file
	*/
	if (!WriteFile(hFile, TempArray, 36, &dwTempSize, nullptr))
	{
		Destroy();
		return false;
	}

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
		dwTemp = SizeInBytes;
		dwTemp2 = dwTemp;
		cbTemp[4] = pTempPointer[0];
		cbTemp[5] = pTempPointer[1];
		cbTemp[6] = pTempPointer[2];
		cbTemp[7] = pTempPointer[3];

		/*
			Write temp data to file
		*/
		if (WriteFile(hFile, cbTemp, 8, &dwTempSize, nullptr))
		{
			WriteFile(hFile, pData, SizeInBytes, &dwWriteDataSize, nullptr);
			FlushFileBuffers(hFile);
		}
	}
}
