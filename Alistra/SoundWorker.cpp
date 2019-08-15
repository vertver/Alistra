#include "Base_Sound.h"
#include "DemoMixer.h"
#include <math.h>

#define ALIGN_SIZE(Size, AlSize)        ((Size + (AlSize-1)) & (~(AlSize-1)))
#define ALIGN_SIZE_64K(Size)            ALIGN_SIZE(Size, 65536)

#define maxmin(a, minimum, maximum)  min(max(a, minimum), maximum)

#if 0
/*
	Windows 10 only
*/
const TIME_INTERVAL Ring09_Intervals[] =
{
	{ 65671, 76777 },		// Piano, F4
	{ 110677, 120487 }		// Piano, E3
};

const TIME_INTERVAL Ring02_Intervals[] =
{
	{ 19028, 24499 },		// Arp, C5
	{ 25342, 30774 },		// Arp, A4
	{ 25342, 36299 },		// Arp, A4-F4-E4
	{ 36030, 47322 },		// Arp, A3-F4-E4
	{ 25342, 47322 }		// Arp, A4-F4-E4 + delay 
};

typedef struct  
{
	int FileId;
	WCHAR szName[32];
} SOUNDID_PATH;

const SOUNDID_PATH SoundsPaths[] = 
{
	{ 0, L"NULL" },
	{ 1, L"Ring09" },
	{ 2, L"Ring02" }
};
#endif

WAVEFORMATEX waveFormat;
HANDLE hFileToPlay = NULL;
float* BaseBuffer = NULL;
float fMasterVolume = 1.f;
DWORD dwHeaderSize = 0;
DWORD dwSampleRate = 0;
DWORD dwSizeToWrite = 0;
size_t BufferPosition = 0;
size_t ProcessedFrames = 0;
size_t FramesCount = 0;

bool
ProcessSoundWorker(
	SOUNDDEVICE_INFO* pInfo
)
{
#if 0
	DWORD dwTemp = 0;
	LARGE_INTEGER larg;
	memset(&larg, 0, sizeof(LARGE_INTEGER));

	hFileToPlay = CreateFileW(L"I:\\Test_Alistra.raw", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (!hFileToPlay || hFileToPlay == INVALID_HANDLE_VALUE) return false;

	GetFileSizeEx(hFileToPlay, &larg);
	if (larg.QuadPart < 8)
	{
		CloseHandle(hFileToPlay);
		hFileToPlay = NULL;
		return false;
	}

	BaseBuffer = (float*)HeapAlloc(GetProcessHeap(), 0, (size_t)larg.QuadPart);
	ReadFile(hFileToPlay, BaseBuffer, (DWORD)larg.QuadPart, &dwTemp, 0);

	FramesCount = (size_t)(larg.QuadPart / sizeof(float));

	CloseHandle(hFileToPlay);	
	hFileToPlay = NULL;

	dwSampleRate = pInfo->Fmt.SampleRate;

#else
	CDemoMixer ThisDemoMixer;

	__try
	{
		DWORD dwShit = 0;
		hFileToPlay = CreateFileW(L"I:\\Alistra_output.raw", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
		if (!hFileToPlay || hFileToPlay == INVALID_HANDLE_VALUE) return false;

		ThisDemoMixer.Initialize(pInfo->Fmt);
		FramesCount = GetMusicFrames(pInfo->Fmt.SampleRate) * pInfo->Fmt.Channels;
		dwSizeToWrite = ALIGN_SIZE_64K(FramesCount * sizeof(float));

		BaseBuffer = (float*)VirtualAlloc(
			NULL, 
			dwSizeToWrite,
			MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
		);
		if (!BaseBuffer) return false;

		while (ProcessedFrames < FramesCount)
		{	
			size_t ProcessFrames = SYNTHBUFFER_SIZE;
			float* pfTemp[2] = {};
			float* pfMix[2] = {};
			float FloatTempBuffer1[SYNTHBUFFER_SIZE] = {};
			float FloatTempBuffer2[SYNTHBUFFER_SIZE] = {};
			float FloatMixBuffer1[SYNTHBUFFER_SIZE] = {};
			float FloatMixBuffer2[SYNTHBUFFER_SIZE] = {};
			pfTemp[0] = FloatTempBuffer1;
			pfTemp[1] = FloatTempBuffer2;
			pfMix[0] = FloatMixBuffer1;
			pfMix[1] = FloatMixBuffer2;

			if (ProcessFrames > FramesCount - ProcessedFrames)
			{
				ProcessFrames = FramesCount - ProcessedFrames;
			}

			ThisDemoMixer.Process(pfTemp, pfMix, ProcessFrames);
			CopyMixToOut(pfMix, &BaseBuffer[ProcessedFrames], 2, pInfo->Fmt.Channels, ProcessFrames);
			
			ProcessedFrames += ProcessFrames * pInfo->Fmt.Channels;
		}
		
		DWORD dwError = GetLastError();
		u64 WriteSizeT = dwSizeToWrite;
		WriteFile(hFileToPlay, BaseBuffer, WriteSizeT, &dwShit, nullptr);
		dwError = GetLastError();
		CloseHandle(hFileToPlay);

 		ThisDemoMixer.Destroy();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		/*
			We can't process our data, so we quit from thread
		*/
		return false;
	}
#endif	

	waveFormat.wFormatTag = (pInfo->Fmt.IsFloat ? 3 : 1);
	waveFormat.wBitsPerSample = pInfo->Fmt.Bits;

	return true;
}

float
GetSoundWorkerProcess()
{
	float ret = (((float)ProcessedFrames) / ((float)(FramesCount)));
	return ret > 0.99f ? 1.0f : ret;
}

DWORD
GetSampleRate()
{
	return dwSampleRate;
}

void 
SoundWorker(
	float* FileData,
	size_t DataSize, 
	int Channels
)
{
	/*
		M$ Frame Size = Single Frame Size * Channels Count

		That means...

		4410 Frames Buffer == 8820 Frames Buffer by normal system of counting frames
	*/
	size_t sizeToRead = DataSize * Channels;

	if (waveFormat.wFormatTag == 3)
	{
		if (BufferPosition + sizeToRead < FramesCount)
		{
			for (size_t i = 0; i < sizeToRead; i++)
			{
				BaseBuffer[BufferPosition + i] *= fMasterVolume;
			}

			memcpy(FileData, &BaseBuffer[BufferPosition], sizeToRead * sizeof(float));
		}
		else
		{
			memset(FileData, 0, sizeToRead * sizeof(float));
		}
	}
	else
	{
		short* pShortData = (short*)FileData;

		switch (waveFormat.wBitsPerSample)
		{
		case 16:
			for (size_t i = 0; i < sizeToRead; i++)
			{
				pShortData[i] = maxmin(((short)(BaseBuffer[BufferPosition + i] * 32768.0f)), -32768, 32767);
			}
			break;
		default:
			break;
		}
	}

	BufferPosition += sizeToRead;
}

bool
IsMusicEnd()
{
	return (BufferPosition >= FramesCount);
} 

void
DestroySoundWorker()
{
	if (BaseBuffer && FramesCount) VirtualFree(BaseBuffer, dwSizeToWrite, MEM_RELEASE);
}
