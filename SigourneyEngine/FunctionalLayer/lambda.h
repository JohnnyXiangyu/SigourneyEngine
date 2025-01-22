#pragma once

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

}
}