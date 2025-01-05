#pragma once

#include "observable.h"
#include "observer.h"
#include "allocator.h"
#include "subscriber_splitter.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace StorageFreeEnumeration {

template <typename T>
class ObservableBase : public IObservable<T>
{
private:
    IObserver<T>* m_observer = nullptr;
    Memory::IAllocator* m_allocator;

protected:
    inline IObserver<T>* GetObserver()  
    {
        return m_observer;
    }

public:
    ObservableBase(Memory::IAllocator* allocator) : m_allocator(allocator) {}

    void Subscribe(IObserver<T>* observer) override
    {
        if (m_observer == nullptr)
        {
            m_observer = observer;
        }
        else
        {
            SubscriberSplitter<T>* newSplitter = m_allocator->New<SubscriberSplitter<T>>();
            newSplitter->SetChildren(m_observer, observer);
            m_observer = newSplitter;
            newSplitter->OnNext(10);
        }
    }
};

}
}
}