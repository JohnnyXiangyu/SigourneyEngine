#pragma once
#include "pch.h"

namespace SigourneyEngine
{
namespace Modules
{
namespace Render2D
{

class Render2DModule
{
public:
	static void* Create(Core::DependencyInjection::ServiceProvider* services);
	static void Update(void* instance, Core::DependencyInjection::ServiceProvider* services);
	static void Finalize(void* instance, Core::DependencyInjection::ServiceProvider* services);

};

}
}
}
