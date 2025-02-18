#include "asset_manager.h"
#include "Logging/logger_service.h"
#include "Configuration/compile_time_flags.h"
#include "Reflection/data_type.h"
#include "Reflection/type_info_registra.h"
#include "asset_reference.h"

#include <nlohmann/json.hpp>
#include <fstream>

using namespace Engine::Core::AssetManagement;

const char AssetManager::s_ChannelName[] = "AssetManager";

// TODO: we need an asset table, module and engine code voluntarily provide methods to create objects from asset data that can be stored in the HIA.
// TODO: need a way to clear loaded assets (e.g. loading a entity), and/or smooth transitions

void AssetManager::LoadJsonString(ByteStream* source)
{
    m_JsonLoadingBuffer.clear();

    char readBuffer[Configuration::STRING_LOAD_BUFFER_SIZE];
    long long newReads = 0;

    while ((newReads = source->Read(readBuffer, Configuration::STRING_LOAD_BUFFER_SIZE - 1)) > 0)
    {
        readBuffer[newReads] = 0;
        m_JsonLoadingBuffer.append(readBuffer);
    }
}


Engine::Core::AssetManagement::AssetManager::~AssetManager()
{
    for (auto& outerPair : m_Cache)
    {
        for (auto& innerPair : outerPair.second)
        {
            if (outerPair.first->SerializerOverride.Disposal != nullptr)
            {
                outerPair.first->SerializerOverride.Disposal(m_ServiceProvider, innerPair.second.Data);
            }
            else
            {
                Disposal(innerPair.second.Data);
            }
        }
    }
}


void* AssetManager::LoadAssetCore(const Reflection::ScriptableType* typeKey, const AssetID& id)
{
    auto foundCache = m_Cache.find(typeKey);
    if (foundCache == m_Cache.end())
    {
        m_Cache.insert(std::make_pair(typeKey, std::unordered_map<AssetID, AssetTableEntry>()));
        foundCache = m_Cache.find(typeKey);
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

    InFileStream bytes(fileStream);

    try
    {
        void* newData = nullptr;

        // deserialize
        if (typeKey->SerializerOverride.Deserialize != nullptr)
        {
            newData = typeKey->SerializerOverride.Deserialize(m_ServiceProvider, (ByteStream*)&bytes);
        }
        else
        {
            newData = AutomaticAssetFactory(typeKey, (ByteStream*)&bytes);
        }

        // optional initialize
        if (typeKey->SerializerOverride.Initialize != nullptr)
        {
            typeKey->SerializerOverride.Initialize(m_ServiceProvider, newData);
        }

        foundCache->second.insert(std::make_pair(id, AssetTableEntry{ newData, true }));
        return newData;
    }
    catch (std::exception& ex)
    {
        Logging::GetLogger()->Error(s_ChannelName, "Asset file failed to deserialize %s, exception: %s, see log above for more information.", id.c_str(), ex.what());
        return nullptr;
    }
}


void* AssetManager::LoadAsset(const std::string& type, const AssetID& id)
{
    const Reflection::ScriptableType* typeKey = Reflection::GetType(type);
    if (typeKey == nullptr)
    {
        Logging::GetLogger()->Error(s_ChannelName, "Asset type not found: %s", type.c_str());
        return nullptr;
    }

    return LoadAssetCore(typeKey, id);
}


void* AssetManager::AutomaticAssetFactory(const Reflection::ScriptableType* typeInfo, ByteStream* source)
{
    void* newAsset = typeInfo->GetCDO(m_Allocator->Malloc(typeInfo->Size));

    LoadJsonString(source);
    auto jsonObject = nlohmann::json::parse(m_JsonLoadingBuffer);

    char* assignerPtr = (char*)newAsset;

    // let exception bubble up, don't handle it here
    for (const Reflection::ScriptableProperty& propertyIterator : typeInfo->Properties)
    {
        auto value = jsonObject.find(propertyIterator.Name);

        switch (propertyIterator.Type)
        {
        case Reflection::DataType::BOOL:
            *((bool*)(assignerPtr + propertyIterator.Offset)) = value->get<bool>();
            break;
        case Reflection::DataType::INT32:
            *((int*)(assignerPtr + propertyIterator.Offset)) = value->get<int>();
            break;
        case Reflection::DataType::UINT32:
            *((unsigned int*)(assignerPtr + propertyIterator.Offset)) = value->get<unsigned int>();
            break;
        case Reflection::DataType::INT64:
            *((long long*)(assignerPtr + propertyIterator.Offset)) = value->get<long long>();
            break;
        case Reflection::DataType::UINT64:
            *((unsigned long long*)(assignerPtr + propertyIterator.Offset)) = value->get<unsigned long long>();
            break;
        case Reflection::DataType::FLOAT:
            *((float*)(assignerPtr + propertyIterator.Offset)) = value->get<float>();
            break;
        case Reflection::DataType::DOUBLE:
            *((double*)(assignerPtr + propertyIterator.Offset)) = value->get<double>();
            break;
            // TODO: serialization for these types
            //case Reflection::DataType::VEC2:
            //	*((int*)(assignerPtr + propertyIterator.Offset)) = value->get<int>();
            //	break;
            //case Reflection::DataType::VEC3:
            //	*((int*)(assignerPtr + propertyIterator.Offset)) = value->get<int>();
            //	break;
            //case Reflection::DataType::VEC4:
            //	*((int*)(assignerPtr + propertyIterator.Offset)) = value->get<int>();
            //	break;
            //case Reflection::DataType::MAT2:
            //	*((int*)(assignerPtr + propertyIterator.Offset)) = value->get<int>();
            //	break;
            //case Reflection::DataType::MAT3:
            //	*((int*)(assignerPtr + propertyIterator.Offset)) = value->get<int>();
            //	break;
            //case Reflection::DataType::MAT4:
            //	*((int*)(assignerPtr + propertyIterator.Offset)) = value->get<int>();
            //	break;
        default:
            Logging::GetLogger()->Error(s_ChannelName, "Unsupported data type %d registered for type %s", (int)propertyIterator.Type, propertyIterator.Name.c_str());

            throw std::runtime_error("Unsupported data type in reflection.");
        }
    }

    for (const Reflection::ReferenceProperty& referenceIterator : typeInfo->References)
    {
        auto value = jsonObject.find(referenceIterator.Name);
        AssetID referencedID = value->get<AssetID>();
        void* referencedAsset = LoadAssetCore(referenceIterator.TargetType, referencedID);
        *((UntypedAssetReference*)(assignerPtr + referenceIterator.Offset)) = { referencedAsset };
    }

    return newAsset;
}
