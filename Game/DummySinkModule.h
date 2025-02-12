#pragma once

#include <DependencyInjection/service_provider.h>
#include <string>

namespace SigourneyEngine
{
namespace Game
{

class DummySinkModule
{
public:
	static void* Create(Core::DependencyInjection::ServiceProvider* services);
	static void Update(void* moduleInstance, Core::DependencyInjection::ServiceProvider* services);
	static void Finalize(void* moduleInstance, Core::DependencyInjection::ServiceProvider* services);
	static std::string Name;

public:
	Core::Logging::LoggerService* Logger = nullptr;
	Core::Memory::HighIntegrityAllocator* Allocator = nullptr;
};

}
}
