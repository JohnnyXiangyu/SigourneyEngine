#pragma once
#include "allocator.h"
#include "enumerable.h"
#include "array_literal.h"
#include "non_essential_macros.h"

namespace SigourneyEngine {
namespace FunctionalLayer {

/// <summary>
/// Base class of one package of runtime resources;
/// it contains necessary runtime utilities including but not limited to a heap allocator.
/// Note: in case of multi-threaded execution, use this class to encapsulate resources for each thread.
/// </summary>
class RuntimeBase
{
    Property(Memory::IAllocator*, Allocator)

public:
    /// <summary>
    /// Util function, allocate a 
    /// </summary>
    /// <typeparam name="TElement"></typeparam>
    /// <typeparam name="TCount"></typeparam>
    /// <returns></returns>
    template <typename TElement, unsigned int TCount>
    Enumeration::ArrayLiteral<TElement, TCount>* MakeArray()
    {
        return GetAllocator()->New<Enumeration::ArrayLiteral<TElement, TCount>>();
    }

public:
    RuntimeBase(Memory::IAllocator* inAllocator) : m_Allocator(inAllocator) {}
};

}
}
