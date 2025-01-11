#pragma once
#include "allocator.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Memory {

/// <summary>
/// A static or "dumb" version of the allocator interface;
/// assuming it's only used in the functional runtime, it allocates memory immediately from the last allocation, 
/// expands by doubling size, and scrubs all memory once a block is returned, assuming a linear calling order.
/// </summary>
class StaticAllocator : public IAllocator
{
private:
    char* Buffer;
    size_t CurrentUsage = 0;
    size_t CurrentSize;

protected:
    /// <summary>
    /// Make sure there's enough room in the buffer, increment the counter and return the previously free pointer.
    /// </summary>
    /// <param name="size"></param>
    /// <returns></returns>
    void* AllocateCore(size_t size) override;

    /// <summary>
    /// Mark this pointer and all buffers allocated after it free.
    /// </summary>
    /// <param name="pointer"></param>
    void DeallocateCore(void* pointer) override;

public:
    StaticAllocator(size_t initialSize);
    ~StaticAllocator();
};

}
}
}
