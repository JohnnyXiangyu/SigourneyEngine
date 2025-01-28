#include "renderer_service.h"
#include "../Logging/logger.h"
#include <SDL.h>

using namespace SigourneyEngine;
using namespace SigourneyEngine::Core;

bool SigourneyEngine::Core::Rendering::RendererService::Configure()
{
	// initialize sdl
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		Logging::Logger::Error("SDL could not initialize! SDL Error:");
		Logging::Logger::Error(SDL_GetError());
		return false;
	}

	m_Initialized = true;

	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		Logging::Logger::Error("Warning: Linear texture filtering not enabled!");
	}

	// TODO: add a configuration service that allows window size to be configured
	m_WindowHeight = 480;
	m_WindowWidth = 640;

	//Create window
	m_Window = SDL_CreateWindow("Sigourney Engine Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_WindowWidth, m_WindowHeight, SDL_WINDOW_SHOWN);
	if (m_Window == NULL)
	{
		Logging::Logger::Error("Window could not be created! SDL Error:");
		Logging::Logger::Error(SDL_GetError());
		return false;
	}

	//Create renderer for window
	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
	if (m_Renderer == NULL)
	{
		Logging::Logger::Error("Renderer could not be created! SDL Error:");
		Logging::Logger::Error(SDL_GetError());
		return false;
	}

	//Initialize renderer color
	SDL_SetRenderDrawColor(m_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	return true;
}

void SigourneyEngine::Core::Rendering::RendererService::Dispose()
{
	if (!m_Initialized)
		return;

	//Destroy window	
	if (m_Renderer != nullptr)
	{
		SDL_DestroyRenderer(m_Renderer);
	}
	
	if (m_Window != nullptr)
	{
		SDL_DestroyWindow(m_Window);
	}
	
	m_Renderer = nullptr;
	m_Window = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}

void SigourneyEngine::Core::Rendering::RendererService::ClearScreen()
{
	SDL_SetRenderDrawColor(m_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(m_Renderer);
}

void SigourneyEngine::Core::Rendering::RendererService::RefreshSceen()
{
	SDL_RenderPresent(m_Renderer);
}

void SigourneyEngine::Core::Rendering::RendererService::Render2DGeometry(const Rendering::Geometry2DTarget& target)
{
	SDL_SetRenderDrawColor(m_Renderer, target.ColorR, target.ColorG, target.ColorB, target.ColorA);
	SDL_Rect outlineRect{ target.PositionX, target.PositionY, target.DimensionX, target.DimensionY };

	switch (target.Shape)
	{
	case Rendering::Shape::Rectangle:
		SDL_RenderDrawRect(m_Renderer, &outlineRect);
		break;
	default:
		Logging::Logger::Warning("Unrecognized shape supplied to renderer services.");
		break;
	}
}
