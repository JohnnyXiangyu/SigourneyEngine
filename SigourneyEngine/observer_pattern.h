#pragma once

#include "observer_header.h"
#include "split_observer.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Enumeration {

template <typename TSource>
void Subscribe(ObservableHeader<TSource>* source, ObserverHeader<TSource>* subscriber, Memory::StaticAllocator* allocator)
{
    if (source->Observer == nullptr)
    {
        source->Observer = subscriber;
        return;
    }
    else
    {
        // allocate a new splitter and assign both children to it
        SplitObserver<TSource>* newSplitter = allocator->New<SplitObserver<TSource>>();
        newSplitter->Initialize(source->Observer, subscriber);

        // assign the new splitter to the source sequence
        source->Observer = (ObserverHeader<TSource>*)newSplitter;
        return;
    }
}

template <typename TSource>
void Notify(ObserverHeader<TSource>* observer, TSource value)
{
    observer->OnNextRoutine((void*)observer, value);
}

}
}
}