#pragma once

#include "allocator.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Memory {

/// <summary>
/// Used to alloate memory during each functional invocation and avoid using new statements individually.
/// </summary>
class TransientAllocator : public IAllocator
{
private:
    char* m_buffer;
    size_t m_currentUsage = 0;
    size_t m_currentSize;    

public:
    TransientAllocator(size_t initialCapacity);
    ~TransientAllocator();

    void* AllocateCore(size_t size) override;
};

}
}
}
