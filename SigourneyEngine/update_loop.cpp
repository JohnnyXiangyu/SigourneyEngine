//// impl file:
#include "update_loop.h"
using namespace SigourneyEngine::FunctionalLayer;
using namespace SigourneyEngine::FunctionalLayer::Enumeration;
using namespace SigourneyEngine::FunctionalLayer::Memory;
using namespace SigourneyEngine::Generated::Functional;
int UpdateLoop::Foobar(SigourneyEngine::FunctionalLayer::ILambda<int, int>* lambda, int arg)
{
    return lambda->Run(this, lambda->Run(this, arg));
}
