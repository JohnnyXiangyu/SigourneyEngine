#pragma once

#include <DependencyInjection/service_provider.h>
#include <string>

namespace Game {

class DummySinkModule
{
public:
	static void* Create(Engine::Core::DependencyInjection::ServiceProvider* services);
	static void Update(void* moduleInstance, Engine::Core::DependencyInjection::ServiceProvider* services);
	static void Finalize(void* moduleInstance, Engine::Core::DependencyInjection::ServiceProvider* services);
	static std::string Name;

public:
	Engine::Core::Logging::LoggerService* Logger = nullptr;
	Engine::Core::Memory::HighIntegrityAllocator* Allocator = nullptr;
};

}
