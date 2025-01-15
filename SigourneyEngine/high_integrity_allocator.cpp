#include "high_integrity_allocator.h"
#include <memory>

// define pure internal data structures
namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Memory {

/// <summary>
/// Allocation table stores metadata of all existing instantiation of the New<> method of H.I.A.
/// </summary>
class AllocationTable
{
private:
    // RTTI and statically collected information:
    struct Entry
    {
        Entry* Next;
        size_t Size;
        unsigned int Index;

        void Destroy()
        {
            if (Next != nullptr)
            {
                Next->Destroy();
            }

            delete Next;
        }
    };

    unsigned int Length = 0;
    Entry* Head = nullptr;

    Entry* Seek(size_t size)
    {
        if (Head == nullptr)
        {
            Head = new Entry{ nullptr, size, 0 };
            Length++;
            return Head;
        }

        if (Head->Size == size)
            return Head;

        Entry* currentNode = Head;
        while (currentNode->Next != nullptr)
        {
            if (currentNode->Next->Size == size)
                return currentNode->Next;

            currentNode = currentNode->Next;
        }

        currentNode->Next = new Entry{ nullptr, size, currentNode->Index + 1 };
        Length++;
        return currentNode->Next;
    }

public:
    ~AllocationTable()
    {
        if (Head == nullptr)
            return;

        Head->Destroy();
        delete Head;
    }

    inline unsigned int RegisterSize(size_t size)
    {
        Entry* metaNode = Seek(size);
        return metaNode->Index;
    }

    inline unsigned int GetTableLength() const
    {
        return Length;
    }

    template <typename TCallback>
    void Foreach(TCallback&& callback)
    {
        for (Entry* tableEntry = Head; tableEntry != nullptr; tableEntry = tableEntry->Next)
        {
            callback(tableEntry->Index, tableEntry->Size);
        }
    }
};

static AllocationTable SingletonAllocationTable;

class BufferChainSegment;

/// <summary>
/// Standard metadata header for each block in a buffer chain.
/// </summary>
struct BufferNodeHeader
{
    BufferNodeHeader* NextFree = nullptr;
    BufferChain* Owner = nullptr;

    // get the pointer to a payload
    void* GetPayload() { return ((char*)this) + sizeof(BufferNodeHeader); }
};

/// <summary>
/// A segment of a chain of buffers.
/// </summary>
struct BufferChainSegment
{
    BufferChainSegment* NextSegment = nullptr;
    unsigned int Count = 0;
};

// single-linked list of free buffers
class BufferChain
{
private:
    template <typename THeader>
    struct PaddedNode
    {
        THeader Header;
        char Buffer;
    };

    static BufferNodeHeader* GetFirstHeaderFromSegment(BufferChainSegment* segment)
    {
        return (BufferNodeHeader*)&((PaddedNode<BufferChainSegment>*) segment)->Buffer;
    }

    static char* GetBufferFromHeader(BufferNodeHeader* header)
    {
        return &((PaddedNode<BufferNodeHeader>*) header)->Buffer;
    }

    // creates a chain segment data structure, and a buffer immediately after it
    BufferChainSegment* CreateNewSegment(unsigned int count, size_t payloadSize)
    {
        // create a buffer whose size is the required amount of nodes PLUS a segment header
        size_t nodeSize = payloadSize + sizeof(BufferNodeHeader);
        size_t chainSize = nodeSize * count;
        size_t totalSize = chainSize + sizeof(BufferChainSegment);
        char* newBuffer = new char[totalSize];

        // initialize the first address into a header
        BufferChainSegment* newSegment = new (newBuffer) BufferChainSegment;
        newSegment->Count = count;

        ResetSegment(newSegment);

        // header is at the beginning of the new memory range
        return newSegment;
    }

    // state starts here:

    BufferChainSegment* HeadSegment = nullptr;
    BufferChainSegment* TailSegment = nullptr;

    BufferNodeHeader* HeadNode = nullptr;

    unsigned int PayloadSize = 0;
    unsigned int Count = 0;

    // it does change internal state, not a const function
    void ResetSegment(BufferChainSegment* newSegment)
    {
        char* newBuffer = (char*)newSegment;
        size_t nodeSize = PayloadSize + sizeof(BufferNodeHeader);

        // initialize first n-1 nodes
        char* nodesBegin = newBuffer + sizeof(BufferChainSegment);
        for (int i = 0; i < newSegment->Count - 1; i++)
        {
            BufferNodeHeader* currentNode = (BufferNodeHeader*)(nodesBegin + nodeSize * i);
            BufferNodeHeader* nextNode = (BufferNodeHeader*)(nodesBegin + nodeSize * i + nodeSize);
            currentNode->NextFree = nextNode;
            currentNode->Owner = this;
        }

        // initialize the last node (idk, it's good to avoid having lots of if-statements?)
        BufferNodeHeader* lastNode = (BufferNodeHeader*)(nodesBegin + nodeSize * newSegment->Count - nodeSize);
        lastNode->NextFree = nullptr;
        lastNode->Owner = this;
    }

public:
    BufferChain(unsigned int payloadSize, unsigned int initialCount) : PayloadSize(payloadSize), Count(initialCount) {}

    void* Take()
    {
        // initialize the first segment
        if (HeadSegment == nullptr)
        {
            // allocate a new segment and initialize the chain on top level
            HeadSegment = CreateNewSegment(Count, PayloadSize);
            TailSegment = HeadSegment;

            // the new segment always supplies its first free node at the beginning of its buffer
            HeadNode = GetFirstHeaderFromSegment(HeadSegment);
        }

        // extend the list
        if (HeadNode == nullptr)
        {
            Count *= 2;
            BufferChainSegment* newSegment = CreateNewSegment(Count, PayloadSize);
            TailSegment->NextSegment = newSegment;
            TailSegment = newSegment;
            HeadNode = GetFirstHeaderFromSegment(TailSegment);
        }

        // increment head and return the previous head
        void* result = HeadNode->GetPayload();
        HeadNode = HeadNode->NextFree;
        return result;
    }

    void Put(BufferNodeHeader* header)
    {
        header->NextFree = HeadNode;
        HeadNode = header;
    }

    void Reset()
    {
        for (BufferChainSegment* segment = HeadSegment; segment != nullptr; segment = segment->NextSegment)
        {
            ResetSegment(segment);
        }

        HeadNode = GetFirstHeaderFromSegment(HeadSegment);
    }

    void Destroy()
    {
        // increment to the next segment then delete the current
        // delete is called on the BufferChainSegment* 
        BufferChainSegment* nextSeg = HeadSegment;
        while (nextSeg != nullptr)
        {
            BufferChainSegment* currentSeg = nextSeg;
            nextSeg = nextSeg->NextSegment;
            delete[] currentSeg;
        }
    }
};

}
}
}

using namespace SigourneyEngine::FunctionalLayer::Memory;

// implement H.I.A data structure

void* SigourneyEngine::FunctionalLayer::Memory::HighIntegrityAllocator::AllocateCore(unsigned int tableEntry)
{
    return m_bufferTable[tableEntry].Take();
}

unsigned int SigourneyEngine::FunctionalLayer::Memory::HighIntegrityAllocator::GetTableEntryCore(unsigned int size)
{
    return SingletonAllocationTable.RegisterSize(size);
}

SigourneyEngine::FunctionalLayer::Memory::HighIntegrityAllocator::HighIntegrityAllocator(unsigned int initialSize)
    : m_initialBufferItemCount(initialSize)
{
    // create a table on heap for all registered buffer chains
    unsigned int tableLength = SingletonAllocationTable.GetTableLength();
    m_bufferTable = (BufferChain*)new char[sizeof(BufferChain) * tableLength];
    
    // initialize the buffer table by iterating through static RTTI and placement new the buffers accordingly
    SingletonAllocationTable.Foreach([this](unsigned int index, size_t size) { new (m_bufferTable + index) BufferChain(size, m_initialBufferItemCount); });
}

SigourneyEngine::FunctionalLayer::Memory::HighIntegrityAllocator::~HighIntegrityAllocator()
{
    unsigned int tableLength = SingletonAllocationTable.GetTableLength();
    for (unsigned int i = 0; i < tableLength; i++)
    {
        m_bufferTable[i].Destroy();
    }

    delete[] m_bufferTable;
}

void SigourneyEngine::FunctionalLayer::Memory::HighIntegrityAllocator::Free(void* pointer)
{
    BufferNodeHeader* header = (BufferNodeHeader*)((char*)pointer - sizeof(BufferNodeHeader));
    header->Owner->Put(header);
}
