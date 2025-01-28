#include "module_manager.h"
#include "../Logging/logger.h"
#include "../Utils/non_essential_macros.h"

using namespace SigourneyEngine::Core;

struct Modularization::ModuleManager::SinkModuleInstance
{
	SinkModuleDescriptor* Descriptor;
	void* Instance;
};

void Modularization::ModuleManager::InitializeModules(DependencyInjection::ServiceProvider* services)
{
	m_Instances = new SinkModuleInstance[s_SinkModuleCount];

	for (unsigned int i = 0; i < s_SinkModuleCount; i++)
	{
		m_Instances[i].Descriptor = &s_SinkModuleDescriptors[i];
		m_Instances[i].Instance = s_SinkModuleDescriptors[i].Create(services);
	}
}

void SigourneyEngine::Core::Modularization::ModuleManager::Update(DependencyInjection::ServiceProvider* services)
{
	for (unsigned int i = 0; i < s_SinkModuleCount; i++)
	{
		m_Instances[i].Descriptor->Update(m_Instances->Instance, services);
	}
}

void Modularization::ModuleManager::Finalize(DependencyInjection::ServiceProvider* services)
{
	for (unsigned int i = 0; i < s_SinkModuleCount; i ++)
	{
		m_Instances[i].Descriptor->Finalize(m_Instances->Instance, services);
	}

	delete m_Instances;
}
