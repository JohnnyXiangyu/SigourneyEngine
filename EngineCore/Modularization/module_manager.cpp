#include "module_manager.h"
#include "Utils/non_essential_macros.h"

using namespace Engine::Core;

void Modularization::ModuleManager::InitializeModules(DependencyInjection::ServiceProvider* services)
{

	// sink modules

	size_t sinkModuleCount = m_SinkModuleDescriptors.size();
	m_SinkModules = new void* [sinkModuleCount];
	for (size_t i = 0; i < sinkModuleCount; i++)
	{
		m_SinkModules[i] = m_SinkModuleDescriptors[i].Create(services);
	}
}

void Engine::Core::Modularization::ModuleManager::Update(DependencyInjection::ServiceProvider* services)
{
	// condition check
	if (!IsInitialized())
	{
		Logging::GetLogger()->Error("ModuleManager", "Module update before initialization!");
		return;
	}

	// sink modules

	size_t sinkModuleCount = m_SinkModuleDescriptors.size();
	for (size_t i = 0; i < sinkModuleCount; i++)
	{
		m_SinkModuleDescriptors[i].Update(m_SinkModules[i], services);
	}
}

void Modularization::ModuleManager::Finalize(DependencyInjection::ServiceProvider* services)
{
	// condition check
	if (!IsInitialized())
	{
		Logging::GetLogger()->Error("ModuleManager", "Module finalization before initialization!");
		return;
	}

	// sink modules

	size_t sinkModuleCount = m_SinkModuleDescriptors.size();
	for (size_t i = 0; i < sinkModuleCount; i++)
	{
		m_SinkModuleDescriptors[i].Finalize(m_SinkModules[i], services);
	}

	delete[] m_SinkModules;
}
