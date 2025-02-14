#pragma once

#include "Logging/logger_service.h"
#include "DependencyInjection/configuration_provider.h"
#include <SDL_video.h>

struct SDL_Window;

namespace Engine
{
namespace Core
{
namespace Platform
{
	/// <summary>
	/// PlatformAccess handles *almost* everything platform-specific.
	/// This is the first service that other services depend on, it's not a client-facing service.
	/// Currently: this is just a SDL wrapper.
	/// </summary>
	class PlatformAccess
	{
		// injected
	private:
		const DependencyInjection::ConfigurationProvider* m_Configs = nullptr;

		// initialized
	private:
		SDL_Window* m_Window = nullptr;
		SDL_GLContext m_Context = nullptr;
		unsigned int m_ShaderProgram;
		int gVertexPos2DLocation;
		unsigned int m_VBO;
		unsigned int m_IBO;

	private:
		bool InitializeSDL();
		bool InitializeGL();
		void Render();

	public:
		PlatformAccess(const DependencyInjection::ConfigurationProvider* configs);
		~PlatformAccess();

		// never should've been called, only for tests
		void TempUpdate();
	};
}
}
}



