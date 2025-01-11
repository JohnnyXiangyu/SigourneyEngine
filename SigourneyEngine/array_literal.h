#pragma once
#include "enumerable.h"
#include "allocator.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Enumeration {

template <typename TElement, unsigned int TCount>
class ArrayLiteral : IEnumerable<TElement>
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

    // Inherited via IEnumerable
    size_t Count() override
    {
        return TCount;
    }
    
    // Inherited via IEnumerable
    IEnumerator* InitializeEnumerator(Memory::IAllocator* allocator) override
    {
        return allocator->New<Enumerator>();
    }

    // Inherited via IEnumerable
    void FinalizeEnumerator(IEnumerator* enumerator, Memory::IAllocator* allocator) override
    {
        allocator->Free(enumerator);
    }

    // Inherited via IEnumerable
    TElement Dereference(IEnumerator* enumerator) override
    {
        Enumerator* casted = (Enumerator*)enumerator;
        return m_elements[casted->Index];
    }

    // Inherited via IEnumerable
    bool IncrementEnumerator(IEnumerator* enumerator) override
    {
        Enumerator* casted = (Enumerator*)enumerator;
        casted->Index++;
        return casted->Index < TCount;
    }
};

}
}
}