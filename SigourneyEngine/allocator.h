#pragma once

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Memory {

class IAllocator
{
protected:
    virtual void* AllocateCore(size_t size) = 0;
    virtual void DeallocateCore(void* pointer) = 0;

public:
    template <typename TTarget>
    TTarget* New()
    {
        void* ensuredBuffer = AllocateCore(sizeof(TTarget));
        return new (ensuredBuffer) TTarget;
    }

    template <typename TTarget>
    void Free(TTarget* pointer)
    {
        DeallocateCore(pointer);
    }
};

}
}
}
