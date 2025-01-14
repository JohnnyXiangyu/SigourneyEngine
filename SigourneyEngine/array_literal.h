#pragma once
#include "enumerable.h"
#include "high_integrity_allocator.h"

// supresses the placement new warning
#include <memory>

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Enumeration {

template <typename TElement, unsigned int TCount>
class ArrayLiteral : public IEnumerable<TElement>
{
private:
    struct Enumerator : IEnumerator
    {
        size_t Index = 0;
    };

private:
    TElement m_elements[TCount];

public:
    ArrayLiteral()
    {
        IEnumerable<TElement>::SetCountable(true);
        IEnumerable<TElement>::SetCount(TCount);
    }

    /// <summary>
    /// Set an element in the internal array.
    /// (We are using the heap for allocation anyways, better to not do extra allocations on stack, just slot them in one by one.)
    /// </summary>
    /// <param name="index"></param>
    /// <param name="value"></param>
    void SetItem(size_t index, const TElement& value)
    {
        m_elements[index] = value;
    }

    void SetItem(size_t index, const TElement&& value)
    {
        m_elements[index] = value;
    }
    
    // Inherited via IEnumerable
    IEnumerator* InitializeEnumerator(Memory::HighIntegrityAllocator* allocator) override
    {
        IEnumerator* newEnumerator = allocator->New<Enumerator>();
        return newEnumerator;
    }

    // Inherited via IEnumerable
    void FinalizeEnumerator(IEnumerator* enumerator, Memory::HighIntegrityAllocator* allocator) override
    {
        allocator->Free(enumerator);
    }

    // Inherited via IEnumerable
    TElement Dereference(IEnumerator* enumerator) override
    {
        Enumerator* casted = (Enumerator*) enumerator;
        return m_elements[casted->Index];
    }

    // Inherited via IEnumerable
    bool IncrementEnumerator(IEnumerator* enumerator) override
    {
        Enumerator * casted = (Enumerator*)enumerator;;
        casted->Index++;
        return casted->Index < TCount;
    }
};

}
}
}
