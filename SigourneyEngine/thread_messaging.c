#include "thread_messaging.h"
#include <math.h>
#include <stdio.h>

static int CalculateSubBufferLength(int initialSize, int index)
{
	return initialSize * pow(2, index);
}

static int CalculateGlobalIndex(int initialSize, int bufferIndex, int subBufferIndex)
{
	int predecessors = 0;
	for (int i = 0; i < bufferIndex; i++)
	{
		predecessors += CalculateSubBufferLength(initialSize, i);
	}
	return predecessors + subBufferIndex;
}

// unsafe: static (private) function that assumes it's called by a thread-safe function
static BOOL Expand(SE_TP_MessageAllocator* allocator)
{
	// either resize or allocate a new storage for the top layer array
	SE_TP_MessageAllocatorNode** newBufferPointers = allocator->TwoLayerFreelistStore == NULL ? 
		calloc(1, sizeof(void*)) : 
		realloc(allocator->TwoLayerFreelistStore, ((unsigned long long)(allocator->Degree) + 1) * sizeof(void*));
	if (newBufferPointers == NULL)
		return FALSE;

	// allocate the memory for that pointer
	int newLength = CalculateSubBufferLength(allocator->InitialSize, allocator->Degree);
	SE_TP_MessageAllocatorNode* newBuffer = calloc(newLength, sizeof(SE_TP_MessageAllocatorNode));
	if (newBuffer == NULL)
		return FALSE;

	// update allocator
	allocator->Degree++;
	allocator->TwoLayerFreelistStore = newBufferPointers;
	newBufferPointers[allocator->Degree - 1] = newBuffer;
	printf("increased allocator degree to %d\n", allocator->Degree);

	// initialize all the new memory, and link it into the existing 
	for (int i = 0; i < newLength; i++)
	{
		ZeroMemory(newBuffer + i, sizeof(SE_TP_MessageAllocatorNode));
		newBuffer[i].Index = CalculateGlobalIndex(allocator->InitialSize, allocator->Degree - 1, i);
		newBuffer[i].Next = (i + 1 < newLength) ? newBuffer + i + 1 : allocator->HeadOfFreelist;
	}

	// point the head to the new head and report success, signifying a valid value is set in the allocator
	allocator->HeadOfFreelist = newBuffer;
	return TRUE;
}

SE_TP_MessageAllocator CreateMessageAllocator(int initialSize)
{
	// set the invalid start state
	SE_TP_MessageAllocator newAllocator = { 0 };
	newAllocator.InitialSize = initialSize;

	// try to create a new mutex
	HANDLE newMutex = CreateMutex(NULL, FALSE, NULL);
	newAllocator.Lock = newMutex;
	if (newAllocator.Lock == NULL)
		return newAllocator;

	// initialize the first block of storage
	Expand(&newAllocator);
	return newAllocator;
}

void DestroyMessageAllocator(SE_TP_MessageAllocator* allocator)
{
	CloseHandle(allocator->Lock);
	for (int i = 0; i < allocator->Degree; i++)
	{
		free(allocator->TwoLayerFreelistStore[i]);
	}
	free(allocator->TwoLayerFreelistStore);

	ZeroMemory(allocator, sizeof(SE_TP_MessageAllocator));
}

SE_TP_ThreadMessage* AllocateNewThreadMessage(SE_TP_MessageAllocator* allocator)
{
	WaitForSingleObject(allocator->Lock, INFINITE);

	// ensure the head is safe to use
	if (allocator->HeadOfFreelist == NULL && !Expand(allocator))
		return NULL;

	// get the head
	SE_TP_MessageAllocatorNode* result = allocator->HeadOfFreelist;
	if (result == NULL)
		return NULL;

	// move the head to the next item
	allocator->HeadOfFreelist = result->Next;
	ReleaseMutex(allocator->Lock);

	return (SE_TP_ThreadMessage*)result;
}

void FreeThreadMessage(SE_TP_MessageAllocator* allocator, SE_TP_ThreadMessage* message)
{
	WaitForSingleObject(allocator->Lock, INFINITE);

	SE_TP_MessageAllocatorNode* expandedNode = (SE_TP_MessageAllocatorNode*)message;
	expandedNode->Next = allocator->HeadOfFreelist;
	allocator->HeadOfFreelist = expandedNode;

	ReleaseMutex(allocator->Lock);
}
