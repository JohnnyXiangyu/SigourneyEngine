#pragma once

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Memory {

struct StaticAllocator
{
    char* Buffer;
    size_t CurrentUsage = 0;
    size_t CurrentSize;

    void* AllocateCore(size_t size);

    StaticAllocator(size_t initialSize);
    ~StaticAllocator();

    template <typename TTarget>
    TTarget* New()
    {
        void* ensuredBuffer = AllocateCore(sizeof(TTarget));
        return new (ensuredBuffer) TTarget;
    }
};

}
}
}
