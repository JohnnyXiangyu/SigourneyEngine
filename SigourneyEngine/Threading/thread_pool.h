#pragma once
#include "thread_messaging.h"
#include<Windows.h>

typedef struct _SE_TP_WorkerControl
{
	int Id;
} SE_TP_WorkerControl;

typedef struct _SE_TP_ThreadPool
{
	SE_TP_MessageAllocator Allocator;
	int Concurrency;
	HANDLE EventPort;
	HANDLE CallbackPort;
	HANDLE* ThreadHandles;
} SE_TP_ThreadPool;

SE_TP_ThreadPool SE_TP_CreateThreadPool(int initialAllocatorSize);
void SE_TP_InitializeWorkers(SE_TP_ThreadPool* pool, int concurrency);

BOOL SE_TP_PostThreadWork(SE_TP_ThreadPool* pool, void (*routine)(void* input), void* data, unsigned long long portKey);
unsigned long long SE_TP_WhenAny(SE_TP_ThreadPool* pool);

void SE_TP_FinalizeThreadPool(SE_TP_ThreadPool* pool);
