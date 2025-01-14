#pragma once
#include "enumerable.h"
#include "high_integrity_allocator.h"
#include "lambda.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Enumeration {

template <typename TSource, typename TResult>
class SelectEnumerable : public IEnumerable<TResult>
{
private:
    IEnumerable<TSource>* m_source;

    // lambda is stored in the enumerable for convenience
    ILambda<TResult, TSource>* m_lambda;

public:
    // support rvalues
    SelectEnumerable(IEnumerable<TSource>* inSource, ILambda<TResult, TSource>* inLambda) : m_source(inSource), m_lambda(inLambda)
    {
        IEnumerable<TResult>::SetCountable(inSource->GetCountable());
        IEnumerable<TResult>::SetCount(inSource->GetCount());
    }

public:
    // Inherited via IEnumerable
    IEnumerator* InitializeEnumerator(Memory::HighIntegrityAllocator* allocator) override
    {
        return m_source->InitializeEnumerator(allocator);
    }

    // Inherited via IEnumerable
    void FinalizeEnumerator(IEnumerator* enumerator, Memory::HighIntegrityAllocator* allocator) override
    {
        // pass through finalize, this enumerable can't be the root allocation
        m_source->FinalizeEnumerator(enumerator, allocator);
    }

    // Inherited via IEnumerable
    TResult Dereference(IEnumerator* enumerator) override
    {
        return m_lambda->Run(m_source->Dereference(enumerator));
    }

    // Inherited via IEnumerable
    bool IncrementEnumerator(IEnumerator* enumerator) override
    {
        return m_source->IncrementEnumerator(enumerator);
    }
};

}
}
}
