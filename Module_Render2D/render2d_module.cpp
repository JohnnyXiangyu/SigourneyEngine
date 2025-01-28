#include "pch.h"
#include "render2d_module.h"

using namespace SigourneyEngine::Modules;
using namespace SigourneyEngine::Core;

void* SigourneyEngine::Modules::Render2D::Render2DModule::Create(DependencyInjection::ServiceProvider* services)
{
    return new Render2DModule;
}

void SigourneyEngine::Modules::Render2D::Render2DModule::Update(void* instance, DependencyInjection::ServiceProvider* services)
{

}

void SigourneyEngine::Modules::Render2D::Render2DModule::Finalize(void* instance, DependencyInjection::ServiceProvider* services)
{

    delete instance;
}
