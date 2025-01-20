//// header file:
#pragma once
#include"runtime.h"
namespace SigourneyEngine {
namespace Generated {
namespace Functional {

class UpdateLoop : public SigourneyEngine::FunctionalLayer::RuntimeBase
{
public:
    int Foobar(SigourneyEngine::FunctionalLayer::ILambda<int, int>* lambda, int arg);
};

}
}
}