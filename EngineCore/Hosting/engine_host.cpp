#include "engine_host.h"
#include <SDL.h>

using namespace Engine::Core::Hosting;

void EngineHost::AddSinkModule(void* (*create)(DependencyInjection::ServiceProvider* services), void(*update)(void* moduleInstance, DependencyInjection::ServiceProvider* services), void(*finalize)(void* moduleInstance, DependencyInjection::ServiceProvider* services), const std::string& name)
{
	m_ModuleManager.AddSink({ create, update, finalize, name });
}

int EngineHost::Run()
{
	// initialize all game states
	try
	{
		m_ModuleManager.InitializeModules(&m_ServiceProvider);
	}
	catch (const std::runtime_error& e)
	{
		Logging::GetLogger()->Error("Engine", "Hosting failed to start: %s", e.what());
		return -1;
	}

	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		// Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			quit = e.type == SDL_QUIT;
		}

		// begin update loop
		m_ServiceProvider.GetPlatformAccess()->BeginFrame();
		m_ModuleManager.Update(&m_ServiceProvider);
		m_ServiceProvider.GetPlatformAccess()->EndFrame();
	}

	// finalize all game states
	m_ModuleManager.Finalize(&m_ServiceProvider);

	return 0;
}
