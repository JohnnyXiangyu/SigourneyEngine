#include "high_integrity_allocator.h"
#include "homogeneous_storage.h"
#include "../Utils/shorthand_functions.h"
#include <memory>

// define pure internal data structures
namespace SigourneyEngine {
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

}
}

using namespace SigourneyEngine::Memory;
using namespace SigourneyEngine::Utils;

// implement H.I.A data structure

struct OwnerPrependedPayload
{
    HomogeneousStorage* Owner;
};

void* SigourneyEngine::Memory::HighIntegrityAllocator::AllocateCore(unsigned int tableEntry)
{
    void* payload = m_bufferTable[tableEntry].Take();
    OwnerPrependedPayload* paddedResult = (OwnerPrependedPayload*) m_bufferTable[tableEntry].Take();
    paddedResult->Owner = m_bufferTable + tableEntry;
    return Utils::SkipHeader<OwnerPrependedPayload>(paddedResult);
}

unsigned int SigourneyEngine::Memory::HighIntegrityAllocator::GetTableEntryCore(unsigned int size)
{
    return SingletonAllocationTable.RegisterSize(size);
}

SigourneyEngine::Memory::HighIntegrityAllocator::HighIntegrityAllocator(unsigned int initialSize)
    : m_initialBufferItemCount(initialSize)
{
    // create a table on heap for all registered buffer chains
    unsigned int tableLength = SingletonAllocationTable.GetTableLength();
    m_bufferTable = (HomogeneousStorage*)new char[sizeof(HomogeneousStorage) * tableLength];
    
    // initialize the buffer table by iterating through static RTTI and placement new the buffers accordingly (the size needs to include an extra pointer, see OwnerPrependedPayload)
    SingletonAllocationTable.Foreach([this](unsigned int index, size_t size) { new (m_bufferTable + index) HomogeneousStorage(size + sizeof(void*), m_initialBufferItemCount); });
}

SigourneyEngine::Memory::HighIntegrityAllocator::~HighIntegrityAllocator()
{
    unsigned int tableLength = SingletonAllocationTable.GetTableLength();
    for (unsigned int i = 0; i < tableLength; i++)
    {
        m_bufferTable[i].Destroy();
    }

    delete[] m_bufferTable;
}

void SigourneyEngine::Memory::HighIntegrityAllocator::Free(void* pointer)
{
    OwnerPrependedPayload* header = GetHeader<OwnerPrependedPayload>(pointer);
    header->Owner->Put(header);
}
