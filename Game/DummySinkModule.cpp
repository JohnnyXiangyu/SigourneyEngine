#include "DummySinkModule.h"
#include "DummyAssetType.h"

using namespace SigourneyEngine;
using namespace SigourneyEngine::Game;

static void* CreateDummyAsset(void* provider, Core::AssetManagement::ByteStream source)
{
    DummySinkModule* moduleObj = (DummySinkModule*)provider;

    int index = 0;
    int values[2]{ 0 };
    char readBuffer[16]{ 0 };

    for (long long readCount = source.Read(readBuffer, 16); readCount > 0; readCount = source.Read(readBuffer, 16))
    {
        for (long long i = 0; i < readCount; i++)
        {
            if (readBuffer[i] == ' ')
            {
                index++;
                if (index > 1)
                {
                    moduleObj->Logger->Error("DummySinkModule", "Ill formatted asset for DummyAssetType");
                    return nullptr;
                }
            }
            else
            {
                values[index] *= 10;
                values[index] += readBuffer[i] - '0';
            }
        }
    }

    return moduleObj->Allocator->New<DummyAssetType>(DummyAssetType { values[0], values[1] });
}

static void DisposeDummyAsset(void* provider, void* asset)
{
    DummySinkModule* moduleObj = (DummySinkModule*)provider;
    moduleObj->Allocator->Free(asset);
}

void* DummySinkModule::Create(Core::DependencyInjection::ServiceProvider* services)
{
    DummySinkModule* newModule = new DummySinkModule;
    newModule->Logger = services->GetLoggerService();
    newModule->Allocator = services->GetGlobalAllocator();

    services->GetLoggerService()->Information("DummySinkModule", "initialized");
    services->GetAssetManager()->RegisterAssetType("DummyAssetType", newModule, CreateDummyAsset, DisposeDummyAsset);

    DummyAssetType* newAsset = (DummyAssetType*) services->GetAssetManager()->LoadAsset("DummyAssetType", "Dummy/d.txt");
    if (newAsset != nullptr)
    {
        services->GetLoggerService()->Information("DummySinkMoudle", "Loaded asset: %d, %d", newAsset->PartA, newAsset->PartB);
    }
    else
    {
        services->GetLoggerService()->Information("DummySinkModule", "Dummy asset not found");
    }

    return newModule;
}

void DummySinkModule::Update(void* moduleInstance, Core::DependencyInjection::ServiceProvider* services) {}

void DummySinkModule::Finalize(void* moduleInstance, Core::DependencyInjection::ServiceProvider* services)
{
    services->GetLoggerService()->Information("DummySinkModule", "finalized");
}

std::string DummySinkModule::Name = "DummySinkModule";
