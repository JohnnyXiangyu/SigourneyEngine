#include "high_integrity_allocator.h"
#include "homogeneous_storage.h"
#include "Utils/shorthand_functions.h"
#include <memory>

using namespace SigourneyEngine::Core::Memory;
using namespace SigourneyEngine::Core::Utils;

// implement H.I.A data structure

struct OwnerPrependedPayload
{
    HomogeneousStorage* Owner;
};


void* HighIntegrityAllocator::AllocateCore(size_t size)
{
    HomogeneousStorage* targetStore = nullptr;
    
    auto foundStorage = m_BufferTable.find(size);
    if (foundStorage != m_BufferTable.end())
    {
        targetStore = foundStorage->second;
    }
    else
    {
        targetStore = new HomogeneousStorage(size + sizeof(void*), m_InitialBufferItemCount);
        m_BufferTable.insert(std::make_pair(size, targetStore));
    }

    OwnerPrependedPayload* paddedResult = (OwnerPrependedPayload*) targetStore->Take();
    paddedResult->Owner = targetStore;
    return Utils::SkipHeader<OwnerPrependedPayload>(paddedResult);
}


SigourneyEngine::Core::Memory::HighIntegrityAllocator::HighIntegrityAllocator(unsigned int initialSize)
    : m_InitialBufferItemCount(initialSize)
{
}


SigourneyEngine::Core::Memory::HighIntegrityAllocator::~HighIntegrityAllocator()
{
    for (auto& pair : m_BufferTable)
    {
        delete pair.second;
    }
}


void SigourneyEngine::Core::Memory::HighIntegrityAllocator::Free(void* pointer)
{
    OwnerPrependedPayload* header = GetHeader<OwnerPrependedPayload>(pointer);
    header->Owner->Put(header);
}
