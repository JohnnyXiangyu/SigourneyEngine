#pragma once

#include "Utils/non_essential_macros.h"

struct SDL_Window;
struct SDL_Renderer;

namespace SigourneyEngine
{
namespace Core
{
namespace Rendering
{

class RendererResource
{
	Property(unsigned int, RendererID)
};

class Shader;
class IndexBuffer;
class VertexArray;

/// <summary>
/// This iteration only uses small boxes;
/// Each has two integers for its dimensions and two integers for its position, and another three integers for its color.
/// TODO: add a render texture in here (texture loading requires a separate asset loading service)
/// </summary>
class RendererService
{
private:
	bool m_Initialized = false;

	int m_WindowHeight = 0;
	int m_WindowWidth = 0;

	SDL_Window* m_Window = nullptr;
	SDL_Renderer* m_Renderer = nullptr;

public:
	bool Configure();
	void Dispose();
	
public:
	void ClearScreen();
	void RefreshSceen();

	void Draw(const VertexArray& vertexBuffer, const IndexBuffer& indexBuffer, const Shader& shader);
};

}
}
}
