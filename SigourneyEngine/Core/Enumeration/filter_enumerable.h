#pragma once

#include "enumerable.h"
#include "../FunctionalLayer/lambda.h"
#include "../Memory/high_integrity_allocator.h"

namespace SigourneyEngine {
namespace Enumeration {

template <typename T>
class FilterEnumerable : public IEnumerable<T>
{
private:
    IEnumerable<T>* m_source;
    FunctionalLayer::ILambda<bool, T>* m_lambda;

    struct Enumerator : IEnumerator
    {
        IEnumerator* Source;
        T Value;

        Enumerator(IEnumerator* inSource, T initValue) : Source(inSource), Value(initValue) {}
    };

public:
    FilterEnumerable(IEnumerable<T>* inSource, FunctionalLayer::ILambda<bool, T>* inLambda) : m_source(inSource), m_lambda(inLambda)
    {
        IEnumerable<T>::SetCountable(false);
    }

public:
    // Inherited via IEnumerable
    IEnumerator* InitializeEnumerator(Memory::HighIntegrityAllocator* allocator) override
    {
        IEnumerator* sourceEnumerator = m_source->InitializeEnumerator(allocator);

        do
        {
            T initValue = m_source->Dereference(sourceEnumerator);
            if (m_lambda->Run(initValue))
            {
                return allocator->New<Enumerator>(sourceEnumerator, initValue);
            }
        } while (m_source->IncrementEnumerator(sourceEnumerator));

        return nullptr;
    }

    // Inherited via IEnumerable
    void FinalizeEnumerator(IEnumerator* enumerator, Memory::HighIntegrityAllocator* allocator) override
    {
        Enumerator* casted = (Enumerator*)enumerator;
        m_source->FinalizeEnumerator(casted->Source, allocator);
        allocator->Free(casted);
    }

    // Inherited via IEnumerable
    T Dereference(IEnumerator* enumerator) override
    {
        return ((Enumerator*)enumerator)->Value;
    }
    
    // Inherited via IEnumerable
    bool IncrementEnumerator(IEnumerator* enumerator) override
    {
        Enumerator* casted = (Enumerator*)enumerator;
        while (m_source->IncrementEnumerator(casted->Source))
        {
            T newValue = m_source->Dereference(casted->Source);
            if (m_lambda->Run(newValue))
            {
                casted->Value = newValue;
                return true;
            }
        }

        return false;
    }
};

}
}
