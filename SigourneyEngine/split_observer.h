#pragma once
#include "observer_header.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Enumeration {

template <typename TSource>
struct SplitObserver
{
    ObserverHeader<TSource> InputHeader;

    ObservableHeader<TSource> LeftChild;
    ObservableHeader<TSource> RightChild;

    static void SplitObserverNotify(void* context, TSource item)
    {
        SplitObserver<TSource>* self = (SplitObserver<TSource>*) context;
        self->LeftChild.Notify(item);
        self->RightChild.Notify(item);
    }

    SplitObserver() : InputHeader(SplitObserverNotify) {}

    void Initialize(ObserverHeader<TSource>* leftChild, ObserverHeader<TSource>* rightChild)
    {
        LeftChild.Observer = leftChild;
        RightChild.Observer = rightChild;
    }
};

}
}
}