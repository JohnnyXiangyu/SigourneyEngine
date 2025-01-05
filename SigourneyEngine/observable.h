#pragma once

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace StorageFreeEnumeration {

template <typename T>
class IObserver;

template <typename T>
class IObservable
{
public:
    virtual void Subscribe(IObserver<T>* observer) = 0;
};

}
}
}