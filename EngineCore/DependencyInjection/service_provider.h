#pragma once

//#include "Rendering/renderer_service.h"
#include "Memory/high_integrity_allocator.h"
#include "configuration_provider.h"
#include "Platform/platform_access.h"
#include "AssetManagement/asset_manager.h"
#include "Rendering/renderer_service.h"

namespace Engine {
namespace Core {
namespace DependencyInjection {

class ServiceProvider
{
private:
	Memory::HighIntegrityAllocator m_GlobalAllocator;
	ConfigurationProvider m_Configurations;
	Platform::PlatformAccess m_PlatformAccess;
	AssetManagement::AssetManager m_AssetManager;
	Rendering::RendererService m_RendererService;

public:
	inline Memory::HighIntegrityAllocator* GetGlobalAllocator() { return &m_GlobalAllocator; }
	inline const ConfigurationProvider* GetConfigurations() { return &m_Configurations; }
	inline Platform::PlatformAccess* GetPlatformAccess() { return &m_PlatformAccess; }
	inline AssetManagement::AssetManager* GetAssetManager() { return &m_AssetManager; }
	inline Rendering::RendererService* GetRenderer() { return &m_RendererService; }


	ServiceProvider() : m_GlobalAllocator(16), m_Configurations(), m_PlatformAccess(&m_Configurations), m_AssetManager(this, &m_GlobalAllocator), m_RendererService() {}
	~ServiceProvider() = default;
};

}
}
}
