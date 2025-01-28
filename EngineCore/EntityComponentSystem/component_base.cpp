#include "component_base.h"

static unsigned int s_ComponentCount = 0;

unsigned int SigourneyEngine::Core::EntityComponentSystem::ComponentRegistra::Register()
{
    return s_ComponentCount++;
}
