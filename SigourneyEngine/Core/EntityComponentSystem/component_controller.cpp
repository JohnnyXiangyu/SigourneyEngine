#include "component_controller.h"

using namespace SigourneyEngine::EntityComponentSystem;

unsigned int ComponentController::s_ComponentCount = 0;

unsigned int SigourneyEngine::EntityComponentSystem::ComponentController::RegisterType()
{
    return s_ComponentCount++;
}
