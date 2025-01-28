#include "service_provider.h"
#include "../Rendering/renderer_service.h"

bool SigourneyEngine::Core::DependencyInjection::ServiceProvider::Configure()
{
	if (!m_RendererService.Configure())
		return false;

	return true;
}

void SigourneyEngine::Core::DependencyInjection::ServiceProvider::Dispose()
{
	m_RendererService.Dispose();
}
