#pragma once

namespace SigourneyEngine
{
namespace FunctionalLayer
{
namespace StorageFreeEnumeration
{

template <typename T>
class IObserver
{
public:
    virtual void OnNext(T value) = 0;
};

}
}
}