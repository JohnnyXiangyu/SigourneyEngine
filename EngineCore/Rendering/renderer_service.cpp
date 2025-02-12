#include "renderer_service.h"

#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string>

using namespace SigourneyEngine;
using namespace SigourneyEngine::Core;

static int s_GlobalOpenglInitialization = 0;

// run before game starts
bool SigourneyEngine::Core::Rendering::RendererService::Configure()
{
	return false;
}


// run after game is over
void SigourneyEngine::Core::Rendering::RendererService::Dispose()
{

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
