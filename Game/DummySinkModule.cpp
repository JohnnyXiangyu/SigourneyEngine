#include "DummySinkModule.h"
#include "DummyAssetType.h"

using namespace Engine::Core;
using namespace Game;


void* DummySinkModule::Create(DependencyInjection::ServiceProvider* services)
{
    DummySinkModule* newModule = new DummySinkModule;
    newModule->Allocator = services->GetGlobalAllocator();

    Logging::GetLogger()->Information("DummySinkModule", "initialized");

    auto* asset = services->GetAssetManager()->LoadAsset<DummyAssetType>("dummy.json");

    return newModule;
}

void DummySinkModule::Update(void* moduleInstance, DependencyInjection::ServiceProvider* services) {}

void DummySinkModule::Finalize(void* moduleInstance, DependencyInjection::ServiceProvider* services)
{
    Logging::GetLogger()->Information("DummySinkModule", "finalized");
}

std::string DummySinkModule::Name = "DummySinkModule";
