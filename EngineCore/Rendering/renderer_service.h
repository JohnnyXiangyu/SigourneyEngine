#pragma once

struct SDL_Window;
struct SDL_Renderer;

namespace SigourneyEngine
{
namespace Core
{
namespace Rendering
{

enum class Shape
{
	Rectangle
};

struct Geometry2DTarget
{
	Shape Shape;
	
	int DimensionX;
	int DimensionY;
	
	int PositionX;
	int PositionY;

	unsigned char ColorR;
	unsigned char ColorG;
	unsigned char ColorB;
	unsigned char ColorA;
};

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
	void Render2DGeometry(const Geometry2DTarget& target);
};

}
}
}
