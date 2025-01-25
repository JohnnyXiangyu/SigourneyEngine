//// impl file:
#include "update_loop.h"
using namespace SigourneyEngine;
using namespace SigourneyEngine::Generated::Functional;
SigourneyEngine::FunctionalLayer::ILambda<SigourneyEngine::FunctionalLayer::ILambda<int, int>*, int>* UpdateLoop::Foobar(int a)
{
    SigourneyEngine::FunctionalLayer::ILambda<SigourneyEngine::FunctionalLayer::ILambda<int, int>*, int>* _var_0 = SigourneyEngine::FunctionalLayer::LambdaConstructor<SigourneyEngine::FunctionalLayer::ILambda<int, int>*, int>::Construct(GetAllocator(), [a, this](int b)
        {
            SigourneyEngine::FunctionalLayer::ILambda<int, int>* _var_1 = SigourneyEngine::FunctionalLayer::LambdaConstructor<int, int>::Construct(GetAllocator(), [a, b, this](int c)
                {
                    return a + b + c;
                });
            return _var_1;
        });
    return _var_0;
}

