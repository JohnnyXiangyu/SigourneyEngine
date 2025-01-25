//// header file:
#pragma once
#include"../FunctionalLayer/runtime.h"
namespace SigourneyEngine {
namespace Generated {
namespace Functional {

class UpdateLoop : public SigourneyEngine::FunctionalLayer::RuntimeBase
{
public:
    SigourneyEngine::FunctionalLayer::ILambda<SigourneyEngine::FunctionalLayer::ILambda<int, int>*, int>* Foobar(int a);
};

}
}
}
