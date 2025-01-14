#include "high_integrity_allocator.h"
#include "allocation_table.h"
#include <memory>

// define pure internal data structures
namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Memory {

class BufferChainSegment;

/// <summary>
/// Standard metadata header for each block in a buffer chain.
/// </summary>
struct BufferNodeHeader
{
    BufferNodeHeader* NextFree = nullptr;
    BufferChainSegment* Owner = nullptr;

    // get the pointer to a payload
    void* GetPayload() { return ((char*)this) + sizeof(BufferNodeHeader); }
};

/// <summary>
/// A segment of a chain of buffers.
/// </summary>
class BufferChainSegment
{
private:
    char* Buffer = nullptr;
    BufferNodeHeader* Head = nullptr;
    BufferChainSegment* NextSegment = nullptr;

    unsigned int Count;
    size_t PayloadSize;

    // TODO: when adding a segment, put the segment directly in the beginning of a new buffer
    BufferChainSegment* AddSegment()
    {
        if (NextSegment != nullptr)
            return NextSegment->AddSegment();
        
        NextSegment = CreateNew(Count * 2, PayloadSize);
        return NextSegment;
    }

    inline size_t CalculateBufferSize() const
    {
        return (PayloadSize + sizeof(BufferNodeHeader)) * Count;
    }

    BufferChainSegment(unsigned int count, size_t payloadSize, char* buffer)
        : Count(count), PayloadSize(payloadSize), Buffer(buffer)
    {
        Reset();
    }

public:
    // creates a chain segment data structure, and a buffer immediately after it
    static BufferChainSegment* CreateNew(unsigned int count, size_t payloadSize)
    {
        size_t bufferSize = (payloadSize + sizeof(BufferNodeHeader)) * count;
        char* newBuffer = new char[bufferSize + sizeof(BufferChainSegment)];
        return new ((void*)newBuffer) BufferChainSegment(count, payloadSize, newBuffer + sizeof(BufferChainSegment));
    }

    void* Take()
    {
        if (Head != nullptr)
        {
            void* result = Head->GetPayload();
            Head = Head->NextFree;
            return result;
        }
        else
        {
            BufferChainSegment* newSegment = AddSegment();
            void* result = newSegment->Head->GetPayload();
            Head = newSegment->Head->NextFree;
            return result;
        }
    }

    void Put(BufferNodeHeader* header)
    {
        header->NextFree = Head;
        Head = header;
    }

    void Reset()
    {
        if (NextSegment != nullptr)
        {
            NextSegment->Reset();
        }

        size_t nodeSize = sizeof(BufferNodeHeader) + PayloadSize;
        size_t initialSize = Count * nodeSize;

        Head = (BufferNodeHeader*)Buffer;

        // initialize nodes
        for (unsigned int i = 0; i < Count - 1; i++)
        {
            BufferNodeHeader* currentHeader = (BufferNodeHeader*)(Buffer + i * nodeSize);
            BufferNodeHeader* nextHeader = (BufferNodeHeader*)(Buffer + i * nodeSize + nodeSize);

            currentHeader->NextFree = nextHeader;
            currentHeader->Owner = this;
        }

        // last node either points to nullptr (this is the last segment) or to the head of the next segment
        BufferNodeHeader* lastNode = (BufferNodeHeader*)(Buffer + Count * nodeSize - nodeSize);
        lastNode->NextFree = (NextSegment == nullptr) ? nullptr : NextSegment->Head;
        lastNode->Owner = this;
    }

    void Destroy()
    {
        if (NextSegment == nullptr)
            return;

        NextSegment->Destroy();
        delete[] (char*)NextSegment;
    }
};

}
}
}

using namespace SigourneyEngine::FunctionalLayer::Memory;

// implement H.I.A data structure

void* SigourneyEngine::FunctionalLayer::Memory::HighIntegrityAllocator::AllocateCore(unsigned int tableEntry, size_t payloadSize)
{
    if (m_bufferTable[tableEntry] == nullptr)
    {
        m_bufferTable[tableEntry] = BufferChainSegment::CreateNew(m_initialBufferItemCount, payloadSize);
    }

    return m_bufferTable[tableEntry]->Take();
}

SigourneyEngine::FunctionalLayer::Memory::HighIntegrityAllocator::HighIntegrityAllocator(unsigned int initialSize)
    : m_initialBufferItemCount(initialSize)
{
    m_bufferTable = new BufferChainSegment * [AllocationTable::GetSingleton()->GetTableLength()] { nullptr };
}

SigourneyEngine::FunctionalLayer::Memory::HighIntegrityAllocator::~HighIntegrityAllocator()
{
    unsigned int count = AllocationTable::GetSingleton()->GetTableLength();
    for (unsigned int i = 0; i < count; i++)
    {
        if (m_bufferTable[i] != nullptr)
        {
            m_bufferTable[i]->Destroy();
            delete[](char*)m_bufferTable[i];
        }
    }

    delete[] m_bufferTable;
}

void SigourneyEngine::FunctionalLayer::Memory::HighIntegrityAllocator::Free(void* pointer)
{
    BufferNodeHeader* header = (BufferNodeHeader*)((char*)pointer - sizeof(BufferNodeHeader));
    header->Owner->Put(header);
}
