#include "DummySinkModule.h"
#include "DummyAssetType.h"

using namespace Engine;
using namespace Game;


void* DummySinkModule::Create(Core::DependencyInjection::ServiceProvider* services)
{
    DummySinkModule* newModule = new DummySinkModule;
    newModule->Allocator = services->GetGlobalAllocator();

    Core::Logging::GetLogger()->Information("DummySinkModule", "initialized");

    auto* asset = services->GetAssetManager()->LoadAsset<DummyAssetType>("dummy.json");

    return newModule;
}

void DummySinkModule::Update(void* moduleInstance, Core::DependencyInjection::ServiceProvider* services) {}

void DummySinkModule::Finalize(void* moduleInstance, Core::DependencyInjection::ServiceProvider* services)
{
    Core::Logging::GetLogger()->Information("DummySinkModule", "finalized");
}

std::string DummySinkModule::Name = "DummySinkModule";
