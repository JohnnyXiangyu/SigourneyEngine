#include "update_loop.h"
using namespace SigourneyEngine::FunctionalLayer;
using namespace SigourneyEngine::FunctionalLayer::Enumeration;
using namespace SigourneyEngine::FunctionalLayer::Memory;
using namespace SigourneyEngine::Generated::Functional;
SigourneyEngine::FunctionalLayer::ILambda<int, int>* UpdateLoop::Foobar(int a)
{
    struct _var_0 : public ILambda<int, int>
    {
        int a;
        int Run(int b) override
        {
            return a + b;
        }
    };
    _var_0* _var_1 = RuntimeBase::CreateLambda<_var_0>();
    _var_1->a = a;
    return _var_1;
}