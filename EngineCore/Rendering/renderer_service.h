#pragma once

#include "Utils/non_essential_macros.h"
#include "Logging/logger_service.h"

#include <string>

struct SDL_Window;
struct SDL_Renderer;

namespace Engine {
namespace Core {
namespace Rendering {

enum class ShaderType
{
	FRAGMENT_SHADER,
	VERTEX_SHADER
};

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

private:
	Logging::LoggerService* m_Logger = Logging::GetLogger();

private:
	bool CheckGLError(const char* function, const char* file, int line);

public:
	RendererService();
	~RendererService();

public:
	void ClearScreen();
	void RefreshSceen();

	void Draw(const VertexArray& vertexBuffer, const IndexBuffer& indexBuffer, const Shader& shader);

	bool CompileShader(const std::string& code, unsigned int& outID, ShaderType type);
	bool LinkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader, unsigned int& outID);

	bool DeleteShader(unsigned int shader);
	bool DeleteProgram(unsigned int program);
};

}
}
}
