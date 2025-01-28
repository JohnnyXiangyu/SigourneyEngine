#include "component_controller.h"

using namespace SigourneyEngine::Core::EntityComponentSystem;

unsigned int ComponentController::s_ComponentCount = 0;

unsigned int SigourneyEngine::Core::EntityComponentSystem::ComponentController::RegisterType()
{
    return s_ComponentCount++;
}
