#pragma once

#include "observerable_base.h"
#include "observer.h"
#include "allocator.h"

namespace SigourneyEngine
{
namespace FunctionalLayer
{
namespace StorageFreeEnumeration
{

template <typename TSource, typename TResult>
class ProjectionBase : public ObservableBase<TResult>, public IObserver<TSource>
{
protected:
    virtual TResult Transform(const TSource& sourceItem) = 0;

public:
    ProjectionBase(Memory::IAllocator* allocator) : ObservableBase<TResult>(allocator){}

    void OnNext(TSource dataItem) override
    {
        if (SigourneyEngine::FunctionalLayer::StorageFreeEnumeration::ObservableBase<TResult>::GetObserver() == nullptr)
            return;

        SigourneyEngine::FunctionalLayer::StorageFreeEnumeration::ObservableBase<TResult>::GetObserver()->OnNext(Transform(dataItem));
    }
};

}
}
}