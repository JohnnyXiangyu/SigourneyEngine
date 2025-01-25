#pragma once

enum class Shape
{
	Circle,
	Rectangle
};

struct RenderTarget
{
	Shape Shape;
	int DimensionX;
	int DimensionY;
	int PositionX;
	int PositionY;
};

/// <summary>
/// This iteration only uses basic geometry types: circles and boxes;
/// Each has two integers for its dimensions and two integers for its position, and another three integers for its color.
/// TODO: add a render texture in here (texture loading requires a separate asset loading service)
/// </summary>
class RendererService
{
public:
	void RenderShape(const RenderTarget& target);
};
