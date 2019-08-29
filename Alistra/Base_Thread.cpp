#include "Base_Thread.h"

THREAD_INFO tempInfo;

DWORD
AlistraThreadFunc(
	void* pArgs
)
{
	THREAD_INFO* threadInfo = (THREAD_INFO*)pArgs;

	threadInfo->hThread = GetCurrentThread();
	threadInfo->ThreadId = GetCurrentThreadId();

	SetThreadPriority(threadInfo->hThread, threadInfo->ThreadPriority);
	threadInfo->pFunc(threadInfo->pArgs);

	return 0;
}

void*
BaseCreateThread(
	ThreadFunc* pFunc,
	THREAD_INFO* threadInfo,
	boolean bWait
)
{
	void* hThread = NULL;

	if (!threadInfo)
	{
		memset(&tempInfo, 0, sizeof(THREAD_INFO));
		threadInfo = &tempInfo;
	}

	threadInfo->pFunc = pFunc;

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AlistraThreadFunc, threadInfo, 0, &threadInfo->ThreadId);

	/*
		Waiting for terminating of our thread or struct
	*/
	if (bWait)
	{
		Sleep(5);
	}

	return hThread;
}
