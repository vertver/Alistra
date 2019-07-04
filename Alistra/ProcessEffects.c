#include "Base_Sound.h"
#include <math.h>

/*
	Music Beats = quarter note. That means we can transform it
	to full whole notes (1/1).
*/
#define WHOLE_NOTES_PER_MINUTE (MUSIC_BPM / 4)
#define GetMusicFrames(LSampleRate) ((size_t)(ceilf(((((float)WHOLE_NOTES_COUNT) / ((float)WHOLE_NOTES_PER_MINUTE)) * 60.f) * ((float)(LSampleRate))))) 

#define ALIGN_SIZE(Size, AlSize)        ((Size + (AlSize-1)) & (~(AlSize-1)))
#define ALIGN_SIZE_64K(Size)            ALIGN_SIZE(Size, 65536)

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

float* BaseBuffer = NULL;
size_t BufferPosition = 0;
size_t ProcessedFrames = 0;
size_t FramesCount = 0;

boolean
ProcessSoundWorker(
	SOUNDDEVICE_INFO* pInfo
)
{
	__try
	{
		FramesCount = GetMusicFrames(pInfo->Fmt.SampleRate);
		BaseBuffer = VirtualAlloc(NULL, ALIGN_SIZE_64K(FramesCount * sizeof(float)), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (!BaseBuffer) return false;

		while (ProcessedFrames < FramesCount)
		{
			/*
				TODO: Process function
			*/
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		/*
			We can't process our data, so we quit from thread
		*/
		return false;
	}

	return true;
}

float 
GetSoundWorkerProcess()
{
	float ret = (((float)ProcessedFrames) / ((float)(FramesCount)));
	return ret > 0.95f ? 1.0f : ret;
}

void 
SoundWorker(float* FileData, size_t DataSize)
{
	memcpy(FileData, &BaseBuffer[BufferPosition], DataSize * sizeof(float));

	BufferPosition += DataSize;
}

void
DestroySoundWorker()
{
	if (BaseBuffer && FramesCount) VirtualFree(BaseBuffer, FramesCount * sizeof(float), MEM_RELEASE);
}
