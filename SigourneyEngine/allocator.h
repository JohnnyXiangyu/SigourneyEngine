#pragma once

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Memory {

class IAllocator
{
public:
    virtual void* AllocateCore(size_t size) = 0;

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
