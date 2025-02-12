#pragma once

//#include "Rendering/renderer_service.h"
#include "Memory/high_integrity_allocator.h"
#include "Logging/logger_service.h"
#include "configuration_provider.h"
#include "Platform/platform_access.h"
#include "AssetManagement/asset_manager.h"
#include "Reflection/reflection_manager.h"

namespace SigourneyEngine
{
namespace Core
{
namespace DependencyInjection
{

class ServiceProvider
{
private:
	//Rendering::RendererService m_RendererService;
	Memory::HighIntegrityAllocator m_GlobalAllocator;
	Logging::LoggerService m_LoggerService;
	ConfigurationProvider m_Configurations;
	Platform::PlatformAccess m_PlatformAccess;
	AssetManagement::AssetManager m_AssetManager;
	Reflection::ReflectionManager m_ReflectionManager;

public:
	//Rendering::RendererService* GetRenderer() { return &m_RendererService; }
	inline Memory::HighIntegrityAllocator* GetGlobalAllocator() { return &m_GlobalAllocator; }
	inline Logging::LoggerService* GetLoggerService() { return &m_LoggerService; }
	inline const ConfigurationProvider* GetConfigurations() { return &m_Configurations; }
	inline Platform::PlatformAccess* GetPlatformAccess() { return &m_PlatformAccess; }
	inline AssetManagement::AssetManager* GetAssetManager() { return &m_AssetManager; }
	inline Reflection::ReflectionManager* GetReflectionManager() { return &m_ReflectionManager; }
	
	ServiceProvider() : m_GlobalAllocator(16), m_LoggerService(), m_Configurations(), m_PlatformAccess(&m_LoggerService, &m_Configurations), m_AssetManager(&m_LoggerService, &m_GlobalAllocator), m_ReflectionManager(&m_LoggerService) {}
	~ServiceProvider() = default;
};

}
}
}
