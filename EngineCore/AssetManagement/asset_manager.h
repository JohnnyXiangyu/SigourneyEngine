#pragma once

#include "Memory/high_integrity_allocator.h"
#include "byte_stream.h"
#include "Reflection/scriptable_type.h"
#include "Reflection/type_info_registra.h"


#include <string>
#include <unordered_map>

namespace Engine {
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
	DependencyInjection::ServiceProvider* m_ServiceProvider = nullptr;


	// high-level asset definition
private:
	// TODO: once we get to data builder, take out the json-based implementation
	// TODO: if we keep json, at least make a stream-buffer solution instead of loadin the whole thing in memory
	std::string m_JsonLoadingBuffer;

	void LoadJsonString(ByteStream* source);

	void Disposal(void* asset)
	{
		m_Allocator->Free(asset);
	}

	
	void* AutomaticAssetFactory(const Reflection::ScriptableType* typeInfo, ByteStream* source);


	// dynamic states
private:
	template <typename TAsset>
	friend struct HighLevelAssetDefinition;

	struct AssetTableEntry
	{
		void* Data = nullptr;
		bool Validated = true;
	};

	std::unordered_map<const Reflection::ScriptableType*, std::unordered_map<AssetID, AssetTableEntry>> m_Cache;

	void* LoadAssetCore(const Reflection::ScriptableType* typeKey, const AssetID& id);


public:
	AssetManager(DependencyInjection::ServiceProvider* serviceProvider, Memory::HighIntegrityAllocator* allocator)
		: m_Allocator(allocator), m_ServiceProvider(serviceProvider)
	{
	}

	~AssetManager();

	// TODO: need asset lifetime annotation: right now everything is static scope, eventually there needs to be at least "entity scope" and "module scope"

	/// <summary>
	/// Load an asset by its type name.
	/// Said asset must have been registered before.
	/// </summary>
	/// <param name="type"></param>
	/// <param name="id"></param>
	/// <returns></returns>
	void* LoadAsset(const std::string& type, const AssetID& id);

	template <typename TAsset>
	TAsset* LoadAsset(const AssetID& id)
	{
		return (TAsset*)LoadAssetCore(Reflection::GetType<TAsset>(), id);
	}
};

}
}
}
