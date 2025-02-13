#pragma once

#include "Logging/logger_service.h"
#include "Memory/high_integrity_allocator.h"
#include "byte_stream.h"
#include "Reflection/scriptable_type.h"
#include "Reflection/type_info_registra.h"
#include "nlohmann/json.hpp"
#include "Configuration/compile_time_flags.h"
#include "Reflection/data_type.h"

#include <string>
#include <unordered_map>


// helper of AssetManager::RegisterAssetType to avoid writing an invalid name for a type
#define SE_ASSETS_RegisterAssetType(type) RegisterAssetType<type>(#type)

// helper of AssetManager::LoadAsset to avoid writing an invalid name for a type 
#define SE_ASSETS_LoadAsset(type, id) LoadAsset<type>(#type, id)

namespace SigourneyEngine {
namespace Core {
namespace AssetManagement {

/// <summary>
/// AssetID should be transient to all code; it's only used by entity manager (when loading entities, used to reference resource) and resource loading service (when using AssetID to reference an asset).
/// </summary>
typedef std::string AssetID;


/// <summary>
/// This class manages all resource loading behavior.
/// It's an *INTERNAL* facing interface.
/// </summary>
class AssetManager
{
	static const char s_ChannelName[];

	// dependency injection
private:
	Memory::HighIntegrityAllocator* m_Allocator = nullptr;


	// binary asset definition
private:
	struct AssetTypeInfo
	{
		std::string Name;
		void* Provider;
		void* (*Create)(void* provider, ByteStream source);
		void (*Destroy)(void* provider, void* asset);
	};
	std::unordered_map<std::string, AssetTypeInfo> m_AssetTypes;


	// high-level asset definition
private:
	// TODO: once we get to data builder, take out the json-based implementation
	// TODO: if we keep json, at least make a stream-buffer solution instead of loadin the whole thing in memory
	std::string m_JsonLoadingBuffer;

	void LoadJsonString(ByteStream source);

	static void Disposal(void* provider, void* asset)
	{
		AssetManager* manager = (AssetManager*)provider;
		manager->m_Allocator->Free(asset);
	}

	template <typename TAsset>
	static void* AutomaticAssetFactory(void* provider, ByteStream source)
	{
		AssetManager* manager = (AssetManager*)provider;
		manager->LoadJsonString(source);
		auto jsonObject = nlohmann::json::parse(manager->m_JsonLoadingBuffer);

		TAsset* newAsset = manager->m_Allocator->New<TAsset>();

		char* assignerPtr = (char*)newAsset;

		// let exception bubble up, don't handle it here
		for (const auto& propertyIterator : Reflection::GetType<TAsset>()->Properties)
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

		return newAsset;
	}


	// dynamic states
private:
	template <typename TAsset>
	friend struct HighLevelAssetDefinition;

	struct AssetTableEntry
	{
		void* Data = nullptr;
		bool Validated = true;
	};
	std::unordered_map<std::string, std::unordered_map<AssetID, AssetTableEntry>> m_Cache;


public:
	AssetManager(Memory::HighIntegrityAllocator* allocator)
		: m_Allocator(allocator)
	{
	}

	~AssetManager();

	/// <summary>
	/// Register a type of asset with explicit binary deserializer and disposer.
	/// </summary>
	/// <param name="type">name of the type</param>
	/// <param name="provider">an state object that will be given to the factory method along side data</param>
	/// <param name="factory">factory function</param>
	/// <param name="disposal">disposal function</param>
	void RegisterAssetType(const std::string& type, void* provider, void* (*factory)(void* provider, ByteStream source), void (*disposal)(void* provider, void* asset));

	/// <summary>
	/// Register an asset type using reflection, so assets of this type will be deserialized based on properties registered in reflection manager.
	/// </summary>
	/// <param name="type">both the name that the asset type is registered with *AND* the name to search reflection with</param>
	template <typename TAsset>
	void RegisterAssetType(const std::string& type)
	{
		RegisterAssetType(type, this, AutomaticAssetFactory<TAsset>, Disposal);
	}

	// TODO: high-level asset types are similar to component definitions, so we need a struct definer first
	// void RegisterHighLevelAssetType(const std::string& type, void* provider);

	/// <summary>
	/// Load an asset by its type name.
	/// Said asset must have been registered before.
	/// </summary>
	/// <param name="type"></param>
	/// <param name="id"></param>
	/// <returns></returns>
	void* LoadAsset(const std::string& type, const AssetID& id);

	template <typename TAsset>
	TAsset* LoadAsset(const std::string& type, const AssetID& id)
	{
		return (TAsset*)LoadAsset(type, id);
	}
};

}
}
}
