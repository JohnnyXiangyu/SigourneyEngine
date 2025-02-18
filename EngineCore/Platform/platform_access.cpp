#include "platform_access.h"
#include <gl\glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <stdexcept>

using namespace Engine::Core;
using namespace Engine::Core::Platform;

static const char s_ServiceName[] = "PlatformAccess";
static const char s_SDLInitializationError[] = "SDL initialization failed, see logs for details.";
static const char s_GLInitializationError[] = "OpenGL initialization failed, see logs for details.";

bool Engine::Core::Platform::PlatformAccess::InitializeSDL()
{
	// initialize sdl
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		Logging::GetLogger()->Error(s_ServiceName, "SDL could not initialize!SDL Error: %s", SDL_GetError());
		return false;
	}

	//Use opengl version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//Create window
	m_Window = SDL_CreateWindow("Foobar Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_Configs->WindowWidth, m_Configs->WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (m_Window == NULL)
	{
		Logging::GetLogger()->Error(s_ServiceName, "Window could not be created! SDL Error: %s", SDL_GetError());
		return false;
	}

	//Create context
	m_Context = SDL_GL_CreateContext(m_Window);
	if (m_Context == NULL)
	{
		Logging::GetLogger()->Error(s_ServiceName, "OpenGL context could not be created! SDL Error: %s", SDL_GetError());
		return false;
	}

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		Logging::GetLogger()->Error(s_ServiceName, "Error initializing GLEW! %s", glewGetErrorString(glewError));
		return false;
	}

	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		Logging::GetLogger()->Warning(s_ServiceName, "Warning: Unable to set VSync! SDL Error: %s", SDL_GetError());
	}

	return true;
}

void Engine::Core::Platform::PlatformAccess::BeginFrame()
{
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::Core::Platform::PlatformAccess::EndFrame()
{
	SDL_GL_SwapWindow(m_Window);
}

PlatformAccess::PlatformAccess(const DependencyInjection::ConfigurationProvider* configs)
	: m_Configs(configs)
{
	// initialize SDL
	if (!InitializeSDL())
		throw std::runtime_error(s_SDLInitializationError);
}

Engine::Core::Platform::PlatformAccess::~PlatformAccess()
{
	//Deallocate program
	glDeleteProgram(m_ShaderProgram);

	//Destroy window	
	SDL_DestroyWindow(m_Window);
	m_Window = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}
