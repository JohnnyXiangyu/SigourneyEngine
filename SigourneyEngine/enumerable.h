#pragma once
#include "non_essential_macros.h"

namespace SigourneyEngine {
namespace FunctionalLayer {

namespace Memory {

class HighIntegrityAllocator;

}

namespace Enumeration {

/// <summary>
/// Empty class for better type safety than void*.
/// Not using virtual on these things to safe memory (enumerators are more likely to be created than enumerables.
/// </summary>
struct IEnumerator {};

/// <summary>
/// Interface for all enumerables.
/// The derived class MUST use a parameter-less constructor.
/// </summary>
/// <typeparam name="TElement">type of element</typeparam>
template <typename TElement>
class IEnumerable
{
    Property(bool, Countable)

    Property(size_t, Count)

public:
    /// <summary>
    /// Initialize an enumerator on the allocator.
    /// Returns null if there's nothing in this enumerable.
    /// </summary>
    virtual IEnumerator* InitializeEnumerator(Memory::HighIntegrityAllocator* allocator) = 0;

    /// <summary>
    /// Finalize the enumerator.
    /// </summary>
    virtual void FinalizeEnumerator(IEnumerator* enumerator, Memory::HighIntegrityAllocator* allocator)  = 0;

    /// <summary>
    /// Dereference the value pointed to by the enumerator at its current state.
    /// </summary>
    /// <param name="enumerator"></param>
    virtual TElement Dereference(IEnumerator* enumerator) = 0;

    /// <summary>
    /// Move the enumerator to its next state.
    /// Returns boolean: whether the next state of enumerator is valid.
    /// </summary>
    virtual bool IncrementEnumerator(IEnumerator* enumerator) = 0;
};

}
}
}
