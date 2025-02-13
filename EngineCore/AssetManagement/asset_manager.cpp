#include "asset_manager.h"

#include <fstream>

using namespace SigourneyEngine::Core::AssetManagement;

const char AssetManager::s_ChannelName[] = "AssetManager";

// TODO: we need an asset table, module and engine code voluntarily provide methods to create objects from asset data that can be stored in the HIA.
// TODO: need a way to clear loaded assets (e.g. loading a entity), and/or smooth transitions

void AssetManager::LoadJsonString(ByteStream source)
{
    m_JsonLoadingBuffer.clear();

    char readBuffer[Configuration::STRING_LOAD_BUFFER_SIZE];
    long long newReads = 0;

    while ((newReads = source.Read(readBuffer, Configuration::STRING_LOAD_BUFFER_SIZE - 1)) > 0)
    {
        readBuffer[newReads] = 0;
        m_JsonLoadingBuffer.append(readBuffer);
    }
}


SigourneyEngine::Core::AssetManagement::AssetManager::~AssetManager()
{
    for (auto& outerPair : m_Cache)
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
            Logging::GetLogger()->Warning(s_ChannelName, "Duplicate asset type registration: %s", type.c_str());
        }
    }

    // create new asset type info
    AssetTypeInfo newTypeInfo{ type, provider, factory, disposal };
    m_AssetTypes.insert(std::make_pair(type, newTypeInfo));
    Logging::GetLogger()->Verbose(s_ChannelName, "Registered asset type: %s", type.c_str());
}


void* AssetManager::LoadAsset(const std::string& type, const AssetID& id)
{
    auto foundType = m_AssetTypes.find(type);
    if (foundType == m_AssetTypes.end())
    {
        Logging::GetLogger()->Error(s_ChannelName, "Unrecognized asset type: %s", type.c_str());
        return nullptr;
    }

    auto foundCache = m_Cache.find(type);

    if (foundCache == m_Cache.end())
    {
        m_Cache.insert(std::make_pair(type, std::unordered_map<AssetID, AssetTableEntry>()));
        foundCache = m_Cache.find(type);
    }
    else
    {
        auto foundAsset = foundCache->second.find(id);
        if (foundAsset != foundCache->second.end())
            return foundAsset->second.Data;
    }

    // actually load a new piece of data
    // TODO: replace this when changing serialization system
    Logging::GetLogger()->Verbose(s_ChannelName, "Loading asset: %s", id.c_str());
    std::ifstream fileStream("Assets/" + id);

    if (!fileStream.is_open())
    {
        Logging::GetLogger()->Error(s_ChannelName, "Failed to open asset file %s", id.c_str());
        return nullptr;
    }

    ByteStream bytes(fileStream);

    try
    {
        void* newData = foundType->second.Create(foundType->second.Provider, bytes);
        foundCache->second.insert(std::make_pair(id, AssetTableEntry{ newData, true }));
        return newData;
    }
    catch (std::exception& ex)
    {
        Logging::GetLogger()->Error(s_ChannelName, "Asset file failed to deserialize %s, exception: %s, see log above for more information.", id.c_str(), ex.what());
        return nullptr;
    }


}
