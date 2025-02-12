#include "asset_manager.h"

#include <fstream>

using namespace SigourneyEngine::Core::AssetManagement;

static const char s_ChannelName[] = "AssetManager";

// TODO: we need an asset table, module and engine code voluntarily provide methods to create objects from asset data that can be stored in the HIA.
// TODO: need a way to clear loaded assets (e.g. loading a entity), and/or smooth transitions

AssetManager::AssetManager(Logging::LoggerService* logger, Memory::HighIntegrityAllocator* allocator)
    : m_Logger(logger), m_Allocator(allocator)
{
}

SigourneyEngine::Core::AssetManagement::AssetManager::~AssetManager()
{
    for (auto& outerPair : m_LoadTable)
    {
        for (auto& innerPair : outerPair.second)
        {
            auto assetInfo = m_AssetTypes.find(outerPair.first);
            assetInfo->second.Destroy(assetInfo->second.Provider, innerPair.second.Data);
        }
    }
}


void AssetManager::RegisterAssetType(const std::string& type, void* provider, void* (*factory)(void* provider, ByteStream source), void (*disposal)(void* provider, void* asset))
{
    // avoid duplication
    {
        auto foundTypeInfo = m_AssetTypes.find(type);
        if (foundTypeInfo != m_AssetTypes.end())
        {
            m_Logger->Warning(s_ChannelName, "Duplicate asset type registration: %s", type.c_str());
        }
    }

    // create new asset type info
    AssetTypeInfo newTypeInfo{ type, provider, factory, disposal };
    m_AssetTypes.insert(std::make_pair(type, newTypeInfo));
    m_Logger->Verbose(s_ChannelName, "Registered asset type: %s", type.c_str());
}


void* AssetManager::LoadAsset(const std::string& type, const AssetID& id)
{
    auto foundType = m_AssetTypes.find(type);
    if (foundType == m_AssetTypes.end())
    {
        m_Logger->Error(s_ChannelName, "Unrecognized asset type: %s", type.c_str());
        return nullptr;
    }

    auto foundCache = m_LoadTable.find(type);

    if (foundCache == m_LoadTable.end())
    {
        m_LoadTable.insert(std::make_pair(type, std::unordered_map<AssetID, AssetTableEntry>()));
        foundCache = m_LoadTable.find(type);
    }
    else
    {
        auto foundAsset = foundCache->second.find(id);
        if (foundAsset != foundCache->second.end())
            return foundAsset->second.Data;
    }

    // actually load a new piece of data
    // TODO: replace this when changing serialization system
    m_Logger->Verbose(s_ChannelName, "Loading asset: %s", id.c_str());
    std::ifstream fileStream("Assets/" + id);

    if (!fileStream.is_open())
    {
        m_Logger->Error(s_ChannelName, "Failed to open asset file %s", id.c_str());
        return nullptr;
    }

    ByteStream bytes(fileStream);
    void* newData = foundType->second.Create(foundType->second.Provider, bytes);
    foundCache->second.insert(std::make_pair(id, AssetTableEntry{newData, true}));

    return newData;
}

