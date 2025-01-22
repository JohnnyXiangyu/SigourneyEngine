#pragma once
#include<Windows.h>

// basic data types
typedef enum _SE_TP_MessageType
{
	THREAD_WORKER_MESSAGE_TYPE_RUN,
	THREAD_WORKER_MESSAGE_TYPE_STOP
} SE_TP_MessageType;

typedef struct _SE_TP_ThreadMessageBody
{
	void (*Routine)(void* input);
	void* Data;
	SE_TP_MessageType Type;
	unsigned long long CallbackKey;
} SE_TP_ThreadMessageBody;

typedef struct _SE_TP_ThreadMessage
{
	OVERLAPPED Overlap;
	SE_TP_ThreadMessageBody Body;
} SE_TP_ThreadMessage;

typedef struct _SE_TP_MessageAllocatorNode
{
	SE_TP_ThreadMessage Message;
	struct MessageAllocatorNode* Next;
	int Index;
} SE_TP_MessageAllocatorNode;

/// <summary>
/// use this to take messages from a managed-allocation environment
/// </summary>
typedef struct _SE_TP_MessageAllocator
{
	/// <summary>
	/// the length of TwoLayerFreelistStore
	/// </summary>
	int Degree;

	/// <summary>
	/// the size of the first buffer in this allocator
	/// </summary>
	int InitialSize;

	/// <summary>
	/// lock is used for atomic operations on this object
	/// </summary>
	HANDLE Lock;
	
	/// <summary>
	/// the current head pointer
	/// </summary>
	SE_TP_MessageAllocatorNode* HeadOfFreelist;
	
	/// <summary>
	/// an double array: whenever the store is expanding, 
	/// it allocates a new array and reallocates the top level array only, 
	/// so whoever is using the old value is never affected
	/// </summary>
	SE_TP_MessageAllocatorNode** TwoLayerFreelistStore;
} SE_TP_MessageAllocator;

SE_TP_MessageAllocator CreateMessageAllocator(int initialSize);
void DestroyMessageAllocator(SE_TP_MessageAllocator* allocator);
SE_TP_ThreadMessage* AllocateNewThreadMessage(SE_TP_MessageAllocator* allocator);
void FreeThreadMessage(SE_TP_MessageAllocator* allocator, SE_TP_ThreadMessage* message);
