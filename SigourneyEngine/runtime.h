#pragma once
#include "high_integrity_allocator.h"
#include "enumerable.h"
#include "array_literal.h"
#include "select_enumerable.h"
#include "lambda.h"
#include "non_essential_macros.h"
#include "filter_enumerable.h"

namespace SigourneyEngine {
namespace FunctionalLayer {

/// <summary>
/// Base class of one package of runtime resources;
/// it contains necessary runtime utilities including but not limited to a heap allocator.
/// Note: in case of multi-threaded execution, use this class to encapsulate resources for each thread.
/// </summary>
class RuntimeBase
{
    Property(Memory::HighIntegrityAllocator*, Allocator)

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

    template <typename TSource, typename TResult, typename TLambda>
    Enumeration::SelectEnumerable<TSource, TResult>* Select(Enumeration::IEnumerable<TSource>* source, const TLambda& lambda)
    {
        ILambda<TResult, TSource>* func = GetAllocator()->New<TLambda>(lambda);
        Enumeration::SelectEnumerable<TSource, TResult>* selector = GetAllocator()->New<Enumeration::SelectEnumerable<TSource, TResult>>(source, func);
        return selector;
    }

    template <typename TSource, typename TResult, typename TLambda>
    Enumeration::SelectEnumerable<TSource, TResult>* Select(Enumeration::IEnumerable<TSource>* source, const TLambda&& lambda)
    {
        ILambda<TResult, TSource>* func = GetAllocator()->New<TLambda>(lambda);
        Enumeration::SelectEnumerable<TSource, TResult>* tik = GetAllocator()->New<Enumeration::SelectEnumerable<TSource, TResult>>(source, func);
        return tik;
    }

    template <typename TElement, typename TLambda>
    Enumeration::FilterEnumerable<TElement>* Where(Enumeration::IEnumerable<TElement>* source, TLambda& lambda)
    {
        ILambda<bool, TElement>* func = GetAllocator()->New<TLambda>(lambda);
        Enumeration::FilterEnumerable<TElement>* whereClause = GetAllocator()->New<Enumeration::FilterEnumerable<TElement>>(source, func);
        return whereClause;
    }

    template <typename TElement, typename TLambda>
    Enumeration::FilterEnumerable<TElement>* Where(Enumeration::IEnumerable<TElement>* source, TLambda&& lambda)
    {
        ILambda<bool, TElement>* func = GetAllocator()->New<TLambda>(lambda);
        Enumeration::FilterEnumerable<TElement>* whereClause = GetAllocator()->New<Enumeration::FilterEnumerable<TElement>>(source, func);
        return whereClause;
    }

public:
    RuntimeBase(Memory::HighIntegrityAllocator* inAllocator) : m_Allocator(inAllocator) {}
};

}
}
