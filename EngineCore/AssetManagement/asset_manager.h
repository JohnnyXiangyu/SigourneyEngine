#pragma once

#include "Logging/logger_service.h"
#include "Memory/high_integrity_allocator.h"
#include "byte_stream.h"

#include <string>
#include <unordered_map>

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
	// asset reflection
private:
	struct AssetTypeInfo
	{
		std::string Name;
		void* Provider;
		void* (*Create)(void* provider, ByteStream source);
		void (*Destroy)(void* provider, void* asset);
	};
	std::unordered_map<std::string, AssetTypeInfo> m_AssetTypes;


	// caching mechanism
private:
	struct AssetTableEntry
	{
		void* Data = nullptr;
		bool Validated = true;
	};

	// AssetType ID -> { AssetID -> AssetTableEntry }
	std::unordered_map<std::string, std::unordered_map<AssetID, AssetTableEntry>> m_LoadTable;


	// dependency injection
private:
	Logging::LoggerService* m_Logger = nullptr;
	Memory::HighIntegrityAllocator* m_Allocator = nullptr;


public:
	AssetManager(Logging::LoggerService* logger, Memory::HighIntegrityAllocator* allocator);
	~AssetManager();

	/// <summary>
	/// Register a type of asset.
	/// </summary>
	/// <param name="type">name of the type</param>
	/// <param name="provider">an state object that will be given to the factory method along side data</param>
	/// <param name="factory">factory method</param>
	/// <returns></returns>
	void RegisterAssetType(const std::string& type, void* provider, void* (*factory)(void* provider, ByteStream source), void (*disposal)(void* provider, void* asset));

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
};

}
}
}
