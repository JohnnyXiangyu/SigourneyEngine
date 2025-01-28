#pragma once

#include "../Utils/non_essential_macros.h"
#include "../Rendering/renderer_service.h"
#include "../Memory/high_integrity_allocator.h"
#include "../Logging/logger_service.h"

namespace SigourneyEngine
{
namespace Core
{
namespace DependencyInjection
{

class ServiceProvider
{
private:
	Rendering::RendererService m_RendererService;
	Memory::HighIntegrityAllocator m_GlobalAllocator;
	Logging::LoggerService m_LoggerService;

public:
	bool Configure();
	void Dispose();

	Rendering::RendererService* GetRenderer() { return &m_RendererService; }
	Memory::HighIntegrityAllocator* GetGlobalAllocator() { return &m_GlobalAllocator; }
	Logging::LoggerService* GetLoggerService() { return &m_LoggerService; }
	
	ServiceProvider() : m_RendererService(), m_GlobalAllocator(16), m_LoggerService() {}
};

}
}
}
