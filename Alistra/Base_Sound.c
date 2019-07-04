#include "Base_Sound.h"
#include "Base_Thread.h"
#include <functiondiscoverykeys.h>
#include <audiopolicy.h>
#include <AudioClient.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#ifndef _AVRT_ 
typedef enum _AVRT_PRIORITY
{
	AVRT_PRIORITY_LOW = -1,
	AVRT_PRIORITY_NORMAL,
	AVRT_PRIORITY_HIGH,
	AVRT_PRIORITY_CRITICAL
} AVRT_PRIORITY, *PAVRT_PRIORITY;
#endif

#define _GetProc(fun, type, name)  {                                                        \
                                        fun = (type) GetProcAddress(hDInputDLL,name);       \
                                        if (fun == NULL)									\
										{													\
											DestroySound();									\
                                            return false;                                   \
                                        }                                                   \
                                    }                                                       \

// "1CB9AD4C-DBFA-4c32-B178-C2F568A703B2"
DEFINE_IID(IAudioClient, 1cb9ad4c, dbfa, 4c32, b1, 78, c2, f5, 68, a7, 03, b2);
// "726778CD-F60A-4EDA-82DE-E47610CD78AA"
DEFINE_IID(IAudioClient2, 726778cd, f60a, 4eda, 82, de, e4, 76, 10, cd, 78, aa);
// "7ED4EE07-8E67-4CD4-8C1A-2B7A5987AD42"
DEFINE_IID(IAudioClient3, 7ed4ee07, 8e67, 4cd4, 8c, 1a, 2b, 7a, 59, 87, ad, 42);
// "1BE09788-6894-4089-8586-9A2A6C265AC5"
DEFINE_IID(IMMEndpoint, 1be09788, 6894, 4089, 85, 86, 9a, 2a, 6c, 26, 5a, c5);
// "A95664D2-9614-4F35-A746-DE8DB63617E6"
DEFINE_IID(IMMDeviceEnumerator, a95664d2, 9614, 4f35, a7, 46, de, 8d, b6, 36, 17, e6);
// "BCDE0395-E52F-467C-8E3D-C4579291692E"
DEFINE_CLSID(IMMDeviceEnumerator, bcde0395, e52f, 467c, 8e, 3d, c4, 57, 92, 91, 69, 2e);
// "F294ACFC-3146-4483-A7BF-ADDCA7C260E2"
DEFINE_IID(IAudioRenderClient, f294acfc, 3146, 4483, a7, bf, ad, dc, a7, c2, 60, e2);
// "C8ADBD64-E71E-48a0-A4DE-185C395CD317"
DEFINE_IID(IAudioCaptureClient, c8adbd64, e71e, 48a0, a4, de, 18, 5c, 39, 5c, d3, 17);
// *2A07407E-6497-4A18-9787-32F79BD0D98F*  Or this??
DEFINE_IID(IDeviceTopology, 2A07407E, 6497, 4A18, 97, 87, 32, f7, 9b, d0, d9, 8f);
// *AE2DE0E4-5BCA-4F2D-AA46-5D13F8FDB3A9*
DEFINE_IID(IPart, AE2DE0E4, 5BCA, 4F2D, aa, 46, 5d, 13, f8, fd, b3, a9);
// *4509F757-2D46-4637-8E62-CE7DB944F57B*
DEFINE_IID(IKsJackDescription, 4509F757, 2D46, 4637, 8e, 62, ce, 7d, b9, 44, f5, 7b);

typedef HANDLE	(WINAPI *FAvSetMmThreadCharacteristics)		(LPCSTR, LPDWORD);
typedef BOOL	(WINAPI *FAvRevertMmThreadCharacteristics)	(HANDLE);
typedef BOOL	(WINAPI *FAvSetMmThreadPriority)			(HANDLE, AVRT_PRIORITY);

HMODULE hDInputDLL = 0;
HANDLE hMMCSS = NULL;
HANDLE hWasapiCloseEvent = NULL;
HANDLE hWasapiThreadEvent = NULL;
HANDLE hSoundWorkEnded = NULL;

FAvSetMmThreadCharacteristics    pAvSetMmThreadCharacteristicsA = NULL;
FAvRevertMmThreadCharacteristics pAvRevertMmThreadCharacteristics = NULL;
FAvSetMmThreadPriority           pAvSetMmThreadPriority = NULL;

typedef struct
{
	SOUNDDEVICE_INFO* pInputDeviceInfo;
	SOUNDDEVICE_INFO* pOutputDeviceInfo;

	IMMDevice* pInputDevice;
	IMMDevice* pOutputDevice;

	IAudioClient* pInputClient;
	IAudioClient* pOutputClient;

	IAudioCaptureClient* pCaptureClient;
	IAudioRenderClient* pRenderClient;

	float* pInputBuffer;
	float* pOutputBuffer;

	size_t Flags;
} WASAPI_DEVICES;

WASAPI_DEVICES InitedDevices;

typedef enum
{
	eNone = 0,
	eEnableInputRecord = 1 << 1
} WasapiFlags;

__forceinline
DWORD
GetSleepTime(
	DWORD Frames,
	DWORD SampleRate
)
{
	REFERENCE_TIME Duration;
	if (!SampleRate) return 0;

	Duration = (REFERENCE_TIME)(10000000.0 * Frames / SampleRate);
	return (DWORD)(Duration / 200000);
}

boolean
SoundWorkerIsEnded()
{
	return (WaitForSingleObject(hSoundWorkEnded, 0) == WAIT_OBJECT_0);
}


void
SoundWorkerProc( 
	void* pParams
)
{
	boolean* pIsDone = (void*)pParams;

	/*
		Process all sound by sound worker
	*/
	*pIsDone = ProcessSoundWorker(InitedDevices.pOutputDeviceInfo);

	SetEvent(hSoundWorkEnded);
}

boolean
CreateSoundWorker(
	boolean* pIsDoneBool
)
{
	THREAD_INFO thInfo;
	memset(&thInfo, 0, sizeof(THREAD_INFO));

	thInfo.pArgs = (void*)pIsDoneBool;

	return !!BaseCreateThread(SoundWorkerProc, &thInfo, false);
}

void
WasapiThreadProc(
	void* pParams
)
{
	/*
		Set AVRT for WASAPI playing thread
	*/
	DWORD dwTask = 0;
	hMMCSS = pAvSetMmThreadCharacteristicsA("Audio", &dwTask);
	if (!hMMCSS || FAILED(pAvSetMmThreadPriority(hMMCSS, AVRT_PRIORITY_CRITICAL)))
	{
		return;
	}

	/*
		Update WASAPI buffer while close event not setted 
	*/
	while (WaitForSingleObject(hWasapiCloseEvent, 0) != WAIT_OBJECT_0)
	{
		__try
		{
			DWORD NeedyFrames = InitedDevices.pOutputDeviceInfo->Fmt.Frames;
			UINT32 StreamPadding = 0;
			HRESULT hr = 0;
			BYTE* pByte = NULL;

			while (NeedyFrames)
			{
				/*
					Get count of samples who can be updated now
				*/
				hr = InitedDevices.pOutputClient->lpVtbl->GetCurrentPadding(InitedDevices.pOutputClient, &StreamPadding);
				if (FAILED(hr)) { return; }

				INT32 AvailableFrames = InitedDevices.pOutputDeviceInfo->Fmt.Frames;
				AvailableFrames -= StreamPadding;
				AvailableFrames = abs(AvailableFrames);

				if (!AvailableFrames)
				{
					/*
						Sleep a 1/2 of all buffer samples time
					*/
					DWORD dwSleep = GetSleepTime(NeedyFrames, InitedDevices.pOutputDeviceInfo->Fmt.SampleRate);
					dwSleep /= 2;
					Sleep(dwSleep);
					continue;
				}

				if (AvailableFrames > (INT32)NeedyFrames) { AvailableFrames = NeedyFrames; }

				/*
					In this case, "GetBuffer" function can be failed if 
					buffer length is too much for us
				*/
				hr = InitedDevices.pRenderClient->lpVtbl->GetBuffer(InitedDevices.pRenderClient, AvailableFrames, &pByte);
				if (SUCCEEDED(hr) && pByte)
				{
					/*
						Process soundworker and copy data to main buffer
					*/
					SoundWorker((float*)pByte, AvailableFrames);
				}
				else
				{
					/*
						Don't try to destroy device if the buffer is unavailable
					*/
					if (hr == AUDCLNT_E_BUFFER_TOO_LARGE) { continue; }

					goto EndFuck;
				}

				/*
					If we can't release buffer - close invalid host
				*/
				hr = InitedDevices.pRenderClient->lpVtbl->ReleaseBuffer(InitedDevices.pRenderClient, AvailableFrames, 0);
				if (FAILED(hr))
				{
					goto EndFuck;
				}

				NeedyFrames -= AvailableFrames;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			/*
				We can't do anything, just close the audio device
			*/
			goto EndFuck;
		}
	}

EndFuck:
	DestroySoundWorker();
	if (hMMCSS) pAvRevertMmThreadCharacteristics(hMMCSS);
	SetEvent(hWasapiThreadEvent);
}

boolean
InitSound(
	char* OutputId,
	char* InputId
)
{
	PROPVARIANT value = { 0 };
	WAVEFORMATEX waveFormat = { 0 };
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* pEndPoints = NULL;
	IPropertyStore* pProperty = NULL;
	IAudioClient* pTempInputClient = NULL;
	IAudioClient* pTempOutputClient = NULL;
	HRESULT hr = 0;

	hWasapiThreadEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
	hWasapiCloseEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
	hSoundWorkEnded = CreateEventA(NULL, FALSE, FALSE, NULL);

	if (InitedDevices.pInputDeviceInfo)
	{
		InitedDevices.pInputDeviceInfo = HeapAlloc(GetProcessHeap(), 0, sizeof(SOUNDDEVICE_INFO));
		memset(InitedDevices.pInputDeviceInfo, 0, sizeof(SOUNDDEVICE_INFO));
	}

	if (InitedDevices.pOutputDeviceInfo)
	{
		InitedDevices.pOutputDeviceInfo = HeapAlloc(GetProcessHeap(), 0, sizeof(SOUNDDEVICE_INFO));
		memset(InitedDevices.pOutputDeviceInfo, 0, sizeof(SOUNDDEVICE_INFO));
	}

	/*
		Destroy sound device if this inited
	*/
	if (InitedDevices.pInputDevice || InitedDevices.pOutputDevice)
	{
		DestroySound();
		memset(&InitedDevices, 0, sizeof(WASAPI_DEVICES));
	}

	/*
		Load AVRT stuff to play audio
	*/
	if (!hDInputDLL)
	{
		hDInputDLL = GetModuleHandleA("avrt.dll");

		if (!hDInputDLL)
		{
			hDInputDLL = LoadLibraryA("avrt.dll");
			if (!hDInputDLL)
			{
				return false;
			}
		}

		/*
			Load proc for AVRT, because we don't want to link at this
		*/
		_GetProc(pAvSetMmThreadCharacteristicsA, FAvSetMmThreadCharacteristics, "AvSetMmThreadCharacteristicsA");
		_GetProc(pAvRevertMmThreadCharacteristics, FAvRevertMmThreadCharacteristics, "AvRevertMmThreadCharacteristics");
		_GetProc(pAvSetMmThreadPriority, FAvSetMmThreadPriority, "AvSetMmThreadPriority");
	}
	
	/*
		Create enumerator for single device object
	*/
	if (SUCCEEDED(CoCreateInstance(&A_CLSID_IMMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, &A_IID_IMMDeviceEnumerator, &deviceEnumerator)))
	{
		if (OutputId)
		{
			WCHAR szString[256];
			memset(szString, 0, sizeof(szString));

			if (MultiByteToWideChar(CP_UTF8, 0, OutputId, strlen(OutputId), szString, 256) <= 0)
			{
				/*
					WASAPI can open device by host id string, so we just copy GUID 
					of device and that works! Maybe :/
				*/
				deviceEnumerator->lpVtbl->GetDevice(deviceEnumerator, szString, &InitedDevices.pOutputDevice);
			}
			else
			{
				MessageBoxW(NULL, L"Something goings wrong with your wide char strings in output sound device.", L"ЕГГОГ", MB_OK | MB_ICONHAND);
				return false;
			}
		}

		if (!InitedDevices.pOutputDevice)
		{
			if (SUCCEEDED(deviceEnumerator->lpVtbl->EnumAudioEndpoints(deviceEnumerator, eRender, DEVICE_STATE_ACTIVE, &pEndPoints)))
			{
				/*
					if all methods failed - set default device
				*/
				if (FAILED(deviceEnumerator->lpVtbl->GetDefaultAudioEndpoint(deviceEnumerator, eRender, eMultimedia, &InitedDevices.pOutputDevice)))
				{
					_RELEASE(deviceEnumerator);
					_RELEASE(pEndPoints);
					return false;
				}
			}
		}

		if (InputId)
		{
			WCHAR szString[256];
			memset(szString, 0, sizeof(szString));

			if (MultiByteToWideChar(CP_UTF8, 0, OutputId, strlen(OutputId), szString, 256) <= 0)
			{
				// note: WASAPI can open device by host id string
				deviceEnumerator->lpVtbl->GetDevice(deviceEnumerator, szString, &InitedDevices.pInputDevice);
			}
			else
			{
				MessageBoxW(NULL, L"Something goings wrong with your wide char strings in input sound device.", L"ЕГГОГ", MB_OK | MB_ICONHAND);
				return false;
			}
		}

		if (!InitedDevices.pInputDevice)
		{
			if (SUCCEEDED(deviceEnumerator->lpVtbl->EnumAudioEndpoints(deviceEnumerator, eCapture, DEVICE_STATE_ACTIVE, &pEndPoints)))
			{
				/*
					if all methods failed - set default device
				*/
				if (FAILED(deviceEnumerator->lpVtbl->GetDefaultAudioEndpoint(deviceEnumerator, eCapture, eConsole, &InitedDevices.pInputDevice)))
				{
					_RELEASE(deviceEnumerator);
					_RELEASE(pEndPoints);
					InitedDevices.Flags |= eEnableInputRecord;
				}
			}
		}

		/*
			Activate input and output device
		*/
		InitedDevices.pOutputDevice->lpVtbl->Activate(InitedDevices.pOutputDevice, &A_IID_IAudioClient, CLSCTX_ALL, NULL, &pTempOutputClient);
		if (InitedDevices.pInputDevice) InitedDevices.pInputDevice->lpVtbl->Activate(InitedDevices.pInputDevice, &A_IID_IAudioClient, CLSCTX_ALL, NULL, &pTempInputClient);

		/*
			Open property store for output device
		*/
		if (SUCCEEDED(InitedDevices.pOutputDevice->lpVtbl->OpenPropertyStore(InitedDevices.pOutputDevice, STGM_READ, &pProperty)))
		{
			PropVariantInit(&value);

			// get WAVEFORMATEX struct
			if (SUCCEEDED(pProperty->lpVtbl->GetValue(pProperty, &PKEY_AudioEngine_DeviceFormat, &value)))
			{
				// note: it can be WAVEFORMAT struct, so we copy by smallest size
				memcpy(&waveFormat, value.blob.pBlobData, min(sizeof(WAVEFORMATEX), (value.blob.cbSize ? value.blob.cbSize : sizeof(WAVEFORMAT))));
			}

			PropVariantClear(&value);
			PropVariantInit(&value);

			// get device name 
			if (SUCCEEDED(pProperty->lpVtbl->GetValue(pProperty, &PKEY_Device_FriendlyName, &value)))
			{
				if (value.vt == VT_LPWSTR)
				{
					// we need to get size of data to allocate
					int StringSize = WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, NULL, 0, NULL, NULL);
					char lpNewString[260];
					memset(lpNewString, 0, sizeof(lpNewString));

					if (StringSize && StringSize < 256)
					{
						// convert to UTF-8
						if (WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, lpNewString, 260, NULL, NULL))
						{
							strcpy_s(InitedDevices.pOutputDeviceInfo->szName, 256, lpNewString);
						}
					}
				}
			}

			PropVariantClear(&value);
			_RELEASE(pProperty);

			InitedDevices.pOutputDeviceInfo->Fmt.Bits = (BYTE)waveFormat.wBitsPerSample;
			InitedDevices.pOutputDeviceInfo->Fmt.Channels = (BYTE)waveFormat.nChannels;	
			InitedDevices.pOutputDeviceInfo->Fmt.SampleRate = waveFormat.nSamplesPerSec;
			InitedDevices.pOutputDeviceInfo->Fmt.IsFloat = true;
		}

		/*
			Open property store for input device
		*/
		if (SUCCEEDED(InitedDevices.pOutputDevice->lpVtbl->OpenPropertyStore(InitedDevices.pOutputDevice, STGM_READ, &pProperty)))
		{
			PropVariantInit(&value);

			// get WAVEFORMATEX struct
			if (SUCCEEDED(pProperty->lpVtbl->GetValue(pProperty, &PKEY_AudioEngine_DeviceFormat, &value)))
			{
				// note: it can be WAVEFORMAT struct, so we copy by smallest size
				memcpy(&waveFormat, value.blob.pBlobData, min(sizeof(WAVEFORMATEX), (value.blob.cbSize ? value.blob.cbSize : sizeof(WAVEFORMAT))));
			}

			PropVariantClear(&value);
			PropVariantInit(&value);

			// get device name 
			if (SUCCEEDED(pProperty->lpVtbl->GetValue(pProperty, &PKEY_Device_FriendlyName, &value)))
			{
				if (value.vt == VT_LPWSTR)
				{
					// we need to get size of data to allocate
					int StringSize = WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, NULL, 0, NULL, NULL);
					char lpNewString[260];
					memset(lpNewString, 0, sizeof(lpNewString));

					if (StringSize && StringSize < 256)
					{
						// convert to UTF-8
						if (WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, lpNewString, 260, NULL, NULL))
						{
							strcpy_s(InitedDevices.pInputDeviceInfo->szName, 256, lpNewString);
						}
					}
				}
			}

			PropVariantClear(&value);
			_RELEASE(pProperty);

			InitedDevices.pInputDeviceInfo->Fmt.Bits = (BYTE)waveFormat.wBitsPerSample;
			InitedDevices.pInputDeviceInfo->Fmt.Channels = (BYTE)waveFormat.nChannels;
			InitedDevices.pInputDeviceInfo->Fmt.SampleRate = waveFormat.nSamplesPerSec;
			InitedDevices.pInputDeviceInfo->Fmt.IsFloat = true;
		}

		/*
			Init capture device 
		*/
		if (pTempInputClient)
		{
			REFERENCE_TIME refTimeDefault = 0;
			REFERENCE_TIME refTimeMin = 0;
			WAVEFORMATEX* pTempWfx = NULL;
			UINT32 BufSize = 0;

			if (SUCCEEDED(pTempInputClient->lpVtbl->GetMixFormat(pTempInputClient, &pTempWfx)))
			{
				// it's can be failed, if device is AC97 
				if (FAILED(pTempInputClient->lpVtbl->GetDevicePeriod(pTempInputClient, &refTimeDefault, &refTimeMin)))
				{
					refTimeDefault = 1000000;			// default device period - 100 msecs
				}

				/*
					Initialize capture client
				*/
				if (SUCCEEDED(pTempInputClient->lpVtbl->Initialize(pTempInputClient, AUDCLNT_SHAREMODE_SHARED, 0, refTimeDefault, 0, pTempWfx, NULL)))
				{
					/*
						Get buffer size for our capture client
					*/
					if (FAILED(pTempInputClient->lpVtbl->GetBufferSize(pTempInputClient, &BufSize)))
					{
						BufSize = (UINT32)((float)InitedDevices.pInputDeviceInfo->Fmt.SampleRate / 10.f);
					}

					InitedDevices.pInputDeviceInfo->Fmt.Frames = BufSize;

					/*
						Init capture service
					*/
					if (FAILED(pTempInputClient->lpVtbl->GetService(pTempInputClient, &A_IID_IAudioCaptureClient, &InitedDevices.pCaptureClient)))
					{
						_RELEASE(pTempInputClient);
						_RELEASE(InitedDevices.pInputDevice);
						CoTaskMemFree(pTempWfx);
						return false;
					}
				}
				else
				{		
					_RELEASE(pTempInputClient);
					_RELEASE(InitedDevices.pInputDevice);
					CoTaskMemFree(pTempWfx);
					return false;
				}

				CoTaskMemFree(pTempWfx);
			}
		}

		/*
			Init render device
		*/
		if (pTempOutputClient)
		{
			REFERENCE_TIME refTimeDefault = 0;
			REFERENCE_TIME refTimeMin = 0;
			WAVEFORMATEX* pTempWfx = NULL;
			UINT32 BufSize = 0;

			if (SUCCEEDED(pTempOutputClient->lpVtbl->GetMixFormat(pTempOutputClient, &pTempWfx)))
			{
				// it's can be failed, if device is AC97 
				if (FAILED(pTempOutputClient->lpVtbl->GetDevicePeriod(pTempOutputClient, &refTimeDefault, &refTimeMin)))
				{
					refTimeDefault = 1000000;			// default device period - 100 msecs
				}

				/*
					Initialize capture client
				*/
				if (SUCCEEDED(pTempOutputClient->lpVtbl->Initialize(pTempOutputClient, AUDCLNT_SHAREMODE_SHARED, 0, refTimeDefault, 0, pTempWfx, NULL)))
				{
					/*
						Get buffer size for our capture client
					*/
					if (FAILED(pTempOutputClient->lpVtbl->GetBufferSize(pTempOutputClient, &BufSize)))
					{
						BufSize = (UINT32)((float)InitedDevices.pOutputDeviceInfo->Fmt.SampleRate / 10.f);
					}

					InitedDevices.pOutputDeviceInfo->Fmt.Frames = BufSize;

					/*
						Init capture service
					*/
					if (FAILED(pTempOutputClient->lpVtbl->GetService(pTempOutputClient, &A_IID_IAudioRenderClient, &InitedDevices.pRenderClient)))
					{
						_RELEASE(pTempOutputClient);
						_RELEASE(InitedDevices.pOutputDevice);
						CoTaskMemFree(pTempWfx);
						return false;
					}
				}
				else
				{
					_RELEASE(pTempOutputClient);
					_RELEASE(InitedDevices.pOutputDevice);
					CoTaskMemFree(pTempWfx);
					return false;
				}

				CoTaskMemFree(pTempWfx);
			}
		}
	}

	pTempInputClient->lpVtbl->QueryInterface(pTempInputClient, &A_IID_IAudioClient, &InitedDevices.pInputClient);
	pTempOutputClient->lpVtbl->QueryInterface(pTempOutputClient, &A_IID_IAudioClient, &InitedDevices.pOutputClient);

	_RELEASE(deviceEnumerator);	
	_RELEASE(pTempInputClient);
	_RELEASE(pTempOutputClient);

	return true;
}

void 
DestroySound()
{
	StopAudio();

	if (InitedDevices.pInputDeviceInfo)
	{
		HeapFree(GetProcessHeap(), 0, InitedDevices.pInputDeviceInfo);
		InitedDevices.pInputDeviceInfo = NULL;
	}

	if (InitedDevices.pOutputDeviceInfo)
	{
		HeapFree(GetProcessHeap(), 0, InitedDevices.pOutputDeviceInfo);
		InitedDevices.pOutputDeviceInfo = NULL;
	}

	_RELEASE(InitedDevices.pCaptureClient);
	_RELEASE(InitedDevices.pRenderClient);
	_RELEASE(InitedDevices.pInputClient);
	_RELEASE(InitedDevices.pOutputClient);
	_RELEASE(InitedDevices.pInputDevice);
	_RELEASE(InitedDevices.pOutputDevice);

	if (InitedDevices.pOutputBuffer)
	{
		HeapFree(GetProcessHeap(), 0, InitedDevices.pOutputBuffer);
		InitedDevices.pOutputBuffer = NULL;
	}

	if (InitedDevices.pInputBuffer)
	{
		HeapFree(GetProcessHeap(), 0, InitedDevices.pInputBuffer);
		InitedDevices.pInputBuffer = NULL;
	}

	if (hWasapiThreadEvent)
	{
		CloseHandle(hWasapiThreadEvent);
		hWasapiThreadEvent = NULL;
	}
	if (hWasapiCloseEvent)
	{
		CloseHandle(hWasapiCloseEvent);
		hWasapiCloseEvent = NULL;
	}
	if (hSoundWorkEnded)
	{
		CloseHandle(hSoundWorkEnded);
		hSoundWorkEnded = NULL;
	}
}

boolean 
PlayAudio()
{
	THREAD_INFO thInfo;
	memset(&thInfo, 0, sizeof(THREAD_INFO));

	/*
		Allocate buffers for WASAPI thread
	*/
	if (InitedDevices.pInputDeviceInfo)
	{
		HeapFree(GetProcessHeap(), 0, InitedDevices.pInputDeviceInfo);
		InitedDevices.pInputDeviceInfo = NULL;
	}

	if (InitedDevices.pOutputDeviceInfo)
	{
		HeapFree(GetProcessHeap(), 0, InitedDevices.pOutputDeviceInfo);
		InitedDevices.pOutputDeviceInfo = NULL;
	}

	InitedDevices.pOutputBuffer = HeapAlloc(GetProcessHeap(), 0, InitedDevices.pOutputDeviceInfo->Fmt.Frames * sizeof(float));

	if (InitedDevices.pInputClient)
	{
		InitedDevices.pInputBuffer = HeapAlloc(GetProcessHeap(), 0, InitedDevices.pInputDeviceInfo->Fmt.Frames * sizeof(float));
	}

	return (!!BaseCreateThread(WasapiThreadProc, &thInfo, false));
}

boolean 
StopAudio()
{
	return false;
}

boolean 
EnumerateInputDevices(
	SOUNDDEVICE_INFO*** pSoundInfos, 
	size_t* DevicesCount
)
{
	UINT CountOfOutputs = 0;
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* pEndPoints = NULL;

	if (SUCCEEDED(CoCreateInstance(&A_CLSID_IMMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, &A_IID_IMMDeviceEnumerator, &deviceEnumerator)))
	{
		deviceEnumerator->lpVtbl->EnumAudioEndpoints(deviceEnumerator, eCapture, DEVICE_STATE_ACTIVE, &pEndPoints);
		pEndPoints->lpVtbl->GetCount(pEndPoints, &CountOfOutputs);
	}
	else
	{
		return false;
	}

	*pSoundInfos = (SOUNDDEVICE_INFO**)HeapAlloc(GetProcessHeap(), 0, sizeof(SOUNDDEVICE_INFO*) * (CountOfOutputs + 1));

	// allocate space for host list
	for (size_t i = 0; i < (CountOfOutputs + 1); i++)
	{
		(*pSoundInfos)[i] = (SOUNDDEVICE_INFO*)HeapAlloc(GetProcessHeap(), 0, sizeof(SOUNDDEVICE_INFO));
	}

	for (size_t i = 0; i < CountOfOutputs; i++)
	{
		WAVEFORMATEX waveFormat = { 0 };
		PROPVARIANT value = { 0 };
		IMMDevice* pDevice = NULL;
		IPropertyStore* pProperty = NULL;
		IAudioClient* pAudioClient = NULL;

		// get device
		pEndPoints->lpVtbl->Item(pEndPoints, i, &pDevice);

		if (pDevice->lpVtbl)
		{
			(*pSoundInfos)[i]->Fmt.Index = (BYTE)i;

			if (SUCCEEDED(pDevice->lpVtbl->Activate(pDevice, &A_IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient)))
			{
				char lpNewString[256];
				LPWSTR lpString = NULL;
				pDevice->lpVtbl->GetId(pDevice, &lpString);

				memset(lpNewString, 0, sizeof(lpNewString));

				// convert to UTF-8
				if (WideCharToMultiByte(CP_UTF8, 0, lpString, -1, lpNewString, 256, NULL, NULL))
				{
					strcpy_s((*pSoundInfos)[i]->szGUID, 256, lpNewString);
				}

				CoTaskMemFree(lpString);

				// get property store for format and device name
				if (SUCCEEDED(pDevice->lpVtbl->OpenPropertyStore(pDevice, STGM_READ, &pProperty)))
				{
					PropVariantInit(&value);

					if (SUCCEEDED(pProperty->lpVtbl->GetValue(pProperty, &PKEY_AudioEngine_DeviceFormat, &value)))
					{
						memcpy(&waveFormat, value.blob.pBlobData, min(sizeof(WAVEFORMATEX), value.blob.cbSize));
					}

					PropVariantClear(&value);
					PropVariantInit(&value);

					if (SUCCEEDED(pProperty->lpVtbl->GetValue(pProperty, &PKEY_Device_FriendlyName, &value)))
					{
						if (value.vt == VT_LPWSTR)
						{
							// we need to get size of data to allocate
							int StringSize = WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, NULL, 0, NULL, NULL);

							if (StringSize && StringSize < sizeof(lpNewString))
							{
								// convert to UTF-8
								if (WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, lpNewString, StringSize, NULL, NULL))
								{
									strcpy_s((*pSoundInfos)[i]->szName, SIZEOF_NAME, lpNewString);
								}
							}
						}

					}

					PropVariantClear(&value);
					_RELEASE(pProperty);
				}

				// we didn't need to have WAVEFORMATEX struct
				(*pSoundInfos)[i]->Fmt.Channels = (BYTE)waveFormat.nChannels;
				(*pSoundInfos)[i]->Fmt.SampleRate = waveFormat.nSamplesPerSec;
				(*pSoundInfos)[i]->Fmt.Bits = (BYTE)waveFormat.wBitsPerSample;

				if (pAudioClient)
				{
					UINT32 HostSize = 0;
					pAudioClient->lpVtbl->GetBufferSize(pAudioClient, &HostSize);

					(*pSoundInfos)[i]->Fmt.Frames = HostSize ? HostSize : waveFormat.nSamplesPerSec / 10;		// 100ms latency
				}
			}
		}

		_RELEASE(pAudioClient);
		_RELEASE(pDevice);
	}

	_RELEASE(deviceEnumerator);
	_RELEASE(pEndPoints);

	*DevicesCount = CountOfOutputs;

	return true;
}


boolean
EnumerateOutputDevices(
	SOUNDDEVICE_INFO*** pSoundInfos,
	size_t* DevicesCount
)
{
	UINT CountOfOutputs = 0;
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* pEndPoints = NULL;

	if (SUCCEEDED(CoCreateInstance(&A_CLSID_IMMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, &A_IID_IMMDeviceEnumerator, &deviceEnumerator)))
	{
		deviceEnumerator->lpVtbl->EnumAudioEndpoints(deviceEnumerator, eRender, DEVICE_STATE_ACTIVE, &pEndPoints);
		pEndPoints->lpVtbl->GetCount(pEndPoints, &CountOfOutputs);
	}
	else
	{
		return false;
	}

	*pSoundInfos = (SOUNDDEVICE_INFO**)HeapAlloc(GetProcessHeap(), 0, sizeof(SOUNDDEVICE_INFO*) * (CountOfOutputs + 1));

	// allocate space for host list
	for (size_t i = 0; i < (CountOfOutputs + 1); i++)
	{
		(*pSoundInfos)[i] = (SOUNDDEVICE_INFO*)HeapAlloc(GetProcessHeap(), 0, sizeof(SOUNDDEVICE_INFO));
	}

	for (size_t i = 0; i < CountOfOutputs; i++)
	{
		WAVEFORMATEX waveFormat = { 0 };
		PROPVARIANT value = { 0 };
		IMMDevice* pDevice = NULL;
		IPropertyStore* pProperty = NULL;
		IAudioClient* pAudioClient = NULL;

		// get device
		pEndPoints->lpVtbl->Item(pEndPoints, i, &pDevice);

		if (pDevice->lpVtbl)
		{
			(*pSoundInfos)[i]->Fmt.Index = (BYTE)i;

			if (SUCCEEDED(pDevice->lpVtbl->Activate(pDevice, &A_IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient)))
			{
				char lpNewString[256];
				LPWSTR lpString = NULL;
				pDevice->lpVtbl->GetId(pDevice, &lpString);

				memset(lpNewString, 0, sizeof(lpNewString));

				// convert to UTF-8
				if (WideCharToMultiByte(CP_UTF8, 0, lpString, -1, lpNewString, 256, NULL, NULL))
				{
					strcpy_s((*pSoundInfos)[i]->szGUID, 256, lpNewString);
				}

				CoTaskMemFree(lpString);

				// get property store for format and device name
				if (SUCCEEDED(pDevice->lpVtbl->OpenPropertyStore(pDevice, STGM_READ, &pProperty)))
				{
					PropVariantInit(&value);

					if (SUCCEEDED(pProperty->lpVtbl->GetValue(pProperty, &PKEY_AudioEngine_DeviceFormat, &value)))
					{
						memcpy(&waveFormat, value.blob.pBlobData, min(sizeof(WAVEFORMATEX), value.blob.cbSize));
					}

					PropVariantClear(&value);
					PropVariantInit(&value);

					if (SUCCEEDED(pProperty->lpVtbl->GetValue(pProperty, &PKEY_Device_FriendlyName, &value)))
					{
						if (value.vt == VT_LPWSTR)
						{
							// we need to get size of data to allocate
							int StringSize = WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, NULL, 0, NULL, NULL);

							if (StringSize && StringSize < sizeof(lpNewString))
							{
								// convert to UTF-8
								if (WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, lpNewString, StringSize, NULL, NULL))
								{
									strcpy_s((*pSoundInfos)[i]->szName, SIZEOF_NAME, lpNewString);
								}
							}
						}

					}

					PropVariantClear(&value);
					_RELEASE(pProperty);
				}

				// we didn't need to have WAVEFORMATEX struct
				(*pSoundInfos)[i]->Fmt.Channels = (BYTE)waveFormat.nChannels;
				(*pSoundInfos)[i]->Fmt.SampleRate = waveFormat.nSamplesPerSec;
				(*pSoundInfos)[i]->Fmt.Bits = (BYTE)waveFormat.wBitsPerSample;

				if (pAudioClient)
				{
					UINT32 HostSize = 0;
					pAudioClient->lpVtbl->GetBufferSize(pAudioClient, &HostSize);

					(*pSoundInfos)[i]->Fmt.Frames = HostSize ? HostSize : waveFormat.nSamplesPerSec / 10;		// 100ms latency
				}
			}
		}

		_RELEASE(pAudioClient);
		_RELEASE(pDevice);
	}

	_RELEASE(deviceEnumerator);
	_RELEASE(pEndPoints);

	*DevicesCount = CountOfOutputs;

	return true;
}
