#pragma once

#include "allocator.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Enumeration {

/// <summary>
/// Header of an observer structure.
/// </summary>
/// <typeparam name="TSource">type of item from the source sequence</typeparam>
template <typename TSource>
struct ObserverHeader
{
    void (*OnNextRoutine)(void* context, TSource item);

    // TODO: this constructor only works when the pointer is explicitly assigned, using initialization list causes a bogus array to be assigned; it's not erally likely a static function has a value bound to instance lifetime?
    ObserverHeader(void (*onNextRoutine)(void* context, TSource item))
    {
        OnNextRoutine = onNextRoutine;
    }

    void Notify(TSource item)
    {
        OnNextRoutine((void*)this, item);
    }
};

template <typename TResult>
struct ObservableHeader
{
    ObserverHeader<TResult>* Observer = nullptr;

    void Notify(TResult item)
    {
        if (Observer == nullptr)
            return;

        Observer->Notify(item);
    }
};

}
}
}
