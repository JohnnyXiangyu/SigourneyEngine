#include "component_controller.h"

using namespace Engine::Core::EntityComponentSystem;

unsigned int ComponentController::s_ComponentCount = 0;

unsigned int Engine::Core::EntityComponentSystem::ComponentController::RegisterType()
{
    return s_ComponentCount++;
}
