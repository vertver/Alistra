#pragma once
#include "Base.h"

typedef void ThreadFunc(void *);

typedef struct tagTHREAD_INFO
{
	DWORD ThreadId;
	DWORD ThreadPriority;
	void* hThread;
	void* pArgs;
	ThreadFunc* pFunc;
	const char* lpThreadName;
} THREAD_INFO, *PTHREAD_INFO;

void*
BaseCreateThread(
	ThreadFunc* pFunc,
	THREAD_INFO* threadInfo,
	boolean bWait
);
