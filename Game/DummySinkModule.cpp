#include "DummySinkModule.h"
#include "DummyAssetType.h"

using namespace SigourneyEngine;
using namespace SigourneyEngine::Game;


void* DummySinkModule::Create(Core::DependencyInjection::ServiceProvider* services)
{
    DummySinkModule* newModule = new DummySinkModule;
    newModule->Logger = services->GetLoggerService();
    newModule->Allocator = services->GetGlobalAllocator();

    services->GetLoggerService()->Information("DummySinkModule", "initialized");

    services->GetReflectionManager()->SE_REFLECTION_BeginTypeDefinition(DummyAssetType)
        .SE_REFLECTION_AddProperty(PartA)
        .SE_REFLECTION_AddProperty(PartB)
        .SE_REFLECTION_Finalize();

    const Core::Reflection::ScriptableType* typeInfo = services->GetReflectionManager()->GetType("DummyAssetType");

    return newModule;
}

void DummySinkModule::Update(void* moduleInstance, Core::DependencyInjection::ServiceProvider* services) {}

void DummySinkModule::Finalize(void* moduleInstance, Core::DependencyInjection::ServiceProvider* services)
{
    services->GetLoggerService()->Information("DummySinkModule", "finalized");
}

std::string DummySinkModule::Name = "DummySinkModule";
