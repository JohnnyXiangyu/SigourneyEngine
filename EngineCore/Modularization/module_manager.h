#pragma once

#include "DependencyInjection/service_provider.h"

#include <vector>
#include <string>

namespace Engine
{
namespace Core
{
namespace Modularization
{

class ModuleManager
{
private:
	struct SinkModuleDescriptor
	{
		void* (*Create)(DependencyInjection::ServiceProvider* services) = nullptr;
		void (*Update)(void* moduleInstance, DependencyInjection::ServiceProvider* services) = nullptr;
		void (*Finalize)(void* moduleInstance, DependencyInjection::ServiceProvider* services) = nullptr;
		std::string Name;
	};

	// configure time
private:
	std::vector<SinkModuleDescriptor> m_SinkModuleDescriptors;
	
	// run time
private:
	void** m_SinkModules = nullptr;
	inline bool IsInitialized() const { return m_SinkModules != nullptr; }

public:
	void InitializeModules(DependencyInjection::ServiceProvider* services);
	void Update(DependencyInjection::ServiceProvider* services);
	void Finalize(DependencyInjection::ServiceProvider* services);

public:
	void AddSink(const SinkModuleDescriptor& newSinkModuleDescriptor)
	{
		m_SinkModuleDescriptors.push_back(newSinkModuleDescriptor);
	}

	void AddSink(const SinkModuleDescriptor&& newSinkModuleDescriptor)
	{
		m_SinkModuleDescriptors.push_back(newSinkModuleDescriptor);
	}
};

}
}
}
