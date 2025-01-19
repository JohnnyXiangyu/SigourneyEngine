#pragma once
#include"runtime.h"
namespace SigourneyEngine {
namespace Generated {
namespace Functional {

class UpdateLoop : public SigourneyEngine::FunctionalLayer::RuntimeBase
{
public:
    SigourneyEngine::FunctionalLayer::ILambda<int, int>* Foobar(int a);
};

}
}
}