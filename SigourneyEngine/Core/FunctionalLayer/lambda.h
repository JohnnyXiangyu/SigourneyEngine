#pragma once

#include "../Memory/high_integrity_allocator.h"

namespace SigourneyEngine {
namespace FunctionalLayer {

class RuntimeBase;

/// <summary>
/// Backend for a lambda in semantic machine with its captures.
/// Compiler will generate concrete classes that implement this interface for each individual lambda.
/// </summary>
/// <typeparam name="TRet"></typeparam>
/// <typeparam name="...TArgs"></typeparam>
template <typename TRet, typename ...TArgs>
struct ILambda
{
    virtual TRet Run(TArgs... values) = 0;
};

template <typename TLambda, typename TRet, typename ... TArgs>
struct LambdaClosure : ILambda<TRet, TArgs ...>
{
	TLambda Lambda;
	LambdaClosure(TLambda&& lambda) : Lambda(lambda) {}

	TRet Run(TArgs ... values) override
	{
		return Lambda(std::forward<TArgs>(values)...);
	}
};

template <typename TRet, typename ...TArgs>
struct LambdaConstructor
{
    template <typename TLambda>
    static ILambda<TRet, TArgs...>* Construct(Memory::HighIntegrityAllocator* allocator, const TLambda&& lambda)
    {
        return (ILambda<TRet, TArgs ...>*) allocator->New<LambdaClosure<TLambda, TRet, TArgs...>>(lambda);
    }
};

}
}
