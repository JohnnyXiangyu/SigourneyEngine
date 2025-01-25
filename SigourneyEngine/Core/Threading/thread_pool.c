#include "thread_pool.h"
#include<stdio.h>

OVERLAPPED g_SharedDummyOverlap = { 0 };

static BOOL SendThreadMessage(SE_TP_ThreadPool* pool, SE_TP_ThreadMessageBody body)
{
	SE_TP_ThreadMessage* newMessage = AllocateNewThreadMessage(&pool->Allocator);
	if (newMessage == NULL)
		return FALSE;

	ZeroMemory(&(newMessage->Overlap), sizeof(OVERLAPPED));
	newMessage->Body = body;
	LPOVERLAPPED overlap = (LPOVERLAPPED)newMessage;
	return PostQueuedCompletionStatus(pool->EventPort, sizeof(SE_TP_ThreadMessage), 0, overlap);
}

static HANDLE CreateBlankPort()
{
	HANDLE messagePort = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		NULL,
		0,
		0
	);

	return messagePort;
}

static BOOL ReportFinish(SE_TP_ThreadPool* pool, unsigned long long callbackKey)
{
	return PostQueuedCompletionStatus(pool->CallbackPort, sizeof(OVERLAPPED), callbackKey, &g_SharedDummyOverlap);
}

static DWORD WINAPI WorkerThreadRoutine(void* input)
{
	SE_TP_ThreadPool* ownerPool = (SE_TP_ThreadPool*)input;

	DWORD readCount;
	LPOVERLAPPED overlappedMsgPointer;
	unsigned long long portKey = 0;

	BOOL running = TRUE;
	while (running)
	{
		// get notification from completion port
		BOOL status = GetQueuedCompletionStatus(ownerPool->EventPort, &readCount, &portKey, &overlappedMsgPointer, INFINITE);
		if (!status)
			break;

		// extract message and free it
		SE_TP_ThreadMessage* message = (SE_TP_ThreadMessage*)overlappedMsgPointer;
		SE_TP_ThreadMessageBody body = message->Body;		
		FreeThreadMessage(&ownerPool->Allocator, message);

		// perform actions
		switch (body.Type)
		{
		case THREAD_WORKER_MESSAGE_TYPE_RUN:
			body.Routine((void*)(intptr_t)message->Body.Data);
			ReportFinish(ownerPool, message->Body.CallbackKey);
			break;
		case THREAD_WORKER_MESSAGE_TYPE_STOP:
			running = FALSE;
			break;
		}
	}

	return 0;
}

SE_TP_ThreadPool SE_TP_CreateThreadPool(int initialAllocatorSize)
{
	SE_TP_ThreadPool newPool = { 0 };
	newPool.Concurrency = -1;

	// create a new completion port for the messages
	newPool.EventPort = CreateBlankPort();
	if (newPool.EventPort == NULL)
		return newPool;

	// create a new completion port for the callbacks
	newPool.CallbackPort = CreateBlankPort();
	if (newPool.CallbackPort == NULL)
		return newPool;

	// create new allocator
	newPool.Allocator = CreateMessageAllocator(initialAllocatorSize);
	return newPool;
}

void SE_TP_InitializeWorkers(SE_TP_ThreadPool* pool, int concurrency)
{
	// create the threads
	int creationCount = 0;
	HANDLE* tempArray = calloc(concurrency, sizeof(HANDLE));
	for (int i = 0; i < concurrency; i++)
	{
		HANDLE thread = INVALID_HANDLE_VALUE;
		DWORD threadId;
		HANDLE newThread = CreateThread(NULL, 0, WorkerThreadRoutine, (void*)pool, 0, &threadId);

		if (newThread != INVALID_HANDLE_VALUE)
		{
			tempArray[creationCount] = newThread;
			creationCount++;
		}
	}
	pool->Concurrency = creationCount;

	// consolidate the threads before return to save some memory
	HANDLE* consolidatedArray = realloc(tempArray, creationCount * sizeof(HANDLE));
	pool->ThreadHandles = (consolidatedArray == NULL) ? tempArray : consolidatedArray;
}

BOOL SE_TP_PostThreadWork(SE_TP_ThreadPool* pool, void (*routine)(void* input), void* data, unsigned long long callbackKey)
{
	// initialize the new message
	SE_TP_ThreadMessageBody newMessage = { 0 };

	newMessage.Type = THREAD_WORKER_MESSAGE_TYPE_RUN;
	newMessage.Routine = routine;
	newMessage.Data = data;
	newMessage.CallbackKey = callbackKey;

	return SendThreadMessage(pool, newMessage);
}

unsigned long long SE_TP_WhenAny(SE_TP_ThreadPool* pool)
{
	DWORD readCount;
	unsigned long long portKey;
	void* dummyPointer = NULL;
	GetQueuedCompletionStatus(pool->CallbackPort, &readCount, &portKey, &dummyPointer, INFINITE);
	return portKey;
}

void SE_TP_FinalizeThreadPool(SE_TP_ThreadPool* pool)
{
	SE_TP_ThreadMessageBody terminator = { 0 };
	terminator.Type = THREAD_WORKER_MESSAGE_TYPE_STOP;

	// terminate threads using special message
	for (int i = 0; i < pool->Concurrency; i++)
	{
		SendThreadMessage(pool, terminator);
	}

	// wait for all threads to quit
	for (int i = 0; i < pool->Concurrency; i++)
	{
		WaitForSingleObject(pool->ThreadHandles[i], INFINITE);
		CloseHandle(pool->ThreadHandles[i]);
	}

	// close the completion ports
	CloseHandle(pool->EventPort);

	// free the dynamically allocated array of threads
	free(pool->ThreadHandles);

	// close the allocator
	DestroyMessageAllocator(&pool->Allocator);
}
