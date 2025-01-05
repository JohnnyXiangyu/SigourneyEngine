#pragma once

#include "observer.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace StorageFreeEnumeration {

template <typename TSource>
class SubscriberSplitter : public IObserver<TSource>
{
private:
    IObserver<TSource>* m_leftChild;
    IObserver<TSource>* m_rightChild;

public:
    void OnNext(TSource value) override
    {
        m_leftChild->OnNext(value);
        m_rightChild->OnNext(value);
    }

    void SetChildren(IObserver<TSource>* left, IObserver<TSource>* right)
    {
        m_leftChild = left;
        m_rightChild = right;
    }
};

}
}
}