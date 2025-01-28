#pragma once

#include "../DependencyInjection/service_provider.h"

namespace SigourneyEngine
{
namespace Core
{
namespace Modularization
{

struct SinkModuleDescriptor
{
	void* (*Create)(DependencyInjection::ServiceProvider* services) = nullptr;
	void (*Update)(void* moduleInstance, DependencyInjection::ServiceProvider* services) = nullptr;
	void (*Finalize)(void* moduleInstance, DependencyInjection::ServiceProvider* services) = nullptr;
};

template <typename TSinkModule>
SinkModuleDescriptor RegisterSink()
{
	return SinkModuleDescriptor{ TSinkModule::Create, TSinkModule::Update, TSinkModule::Finalize };
}

class ModuleManager
{
private:
	static SinkModuleDescriptor* s_SinkModuleDescriptors;
	static unsigned int s_SinkModuleCount;

private:
	struct SinkModuleInstance;
	SinkModuleInstance* m_Instances = nullptr;

public:
	void InitializeModules(DependencyInjection::ServiceProvider* services);
	void Update(DependencyInjection::ServiceProvider* services);
	void Finalize(DependencyInjection::ServiceProvider* services);
};

}
}
}
