#include "platform_access.h"
#include <SDL.h>
#include <gl\glew.h>
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

	//Use OpenGL 4.4 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//Create window
	m_Window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_Configs->WindowWidth, m_Configs->WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

bool PlatformAccess::InitializeGL()
{
	//Generate program
	m_ShaderProgram = glCreateProgram();

	//Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Get vertex source
	const GLchar* vertexShaderSource[] =
	{
		"#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
	};

	//Set vertex source
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

	//Compile vertex source
	glCompileShader(vertexShader);

	//Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE)
	{
		Logging::GetLogger()->Error(s_ServiceName, "Unable to compile vertex shader %d!", vertexShader);
		return false;
	}

	//Attach vertex shader to program
	glAttachShader(m_ShaderProgram, vertexShader);

	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Get fragment source
	const GLchar* fragmentShaderSource[] =
	{
		"#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
	};

	//Set fragment source
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

	//Compile fragment source
	glCompileShader(fragmentShader);

	//Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE)
	{
		Logging::GetLogger()->Error(s_ServiceName, "Unable to compile fragment shader %d!\n", fragmentShader);
		return false;
	}

	//Attach fragment shader to program
	glAttachShader(m_ShaderProgram, fragmentShader);


	//Link program
	glLinkProgram(m_ShaderProgram);

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		printf("Error linking program %d!\n", m_ShaderProgram);
		return false;
	}

	//Get vertex attribute location
	auto m_VertexPos2DLocation = glGetAttribLocation(m_ShaderProgram, "LVertexPos2D");
	if (m_VertexPos2DLocation == -1)
	{
		printf("LVertexPos2D is not a valid glsl program variable!\n");
		return false;
	}

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//VBO data
	GLfloat vertexData[] =
	{
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f,  0.5f,
		-0.5f,  0.5f
	};

	//IBO data
	GLuint indexData[] = { 0, 1, 2, 3 };

	//Create VBO
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, vertexData, GL_STATIC_DRAW);

	//Create IBO
	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

	return true;
}

void Engine::Core::Platform::PlatformAccess::Render()
{
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//Bind program
	glUseProgram(m_ShaderProgram);

	//Enable vertex position
	glEnableVertexAttribArray(gVertexPos2DLocation);

	//Set vertex data
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

	//Set index data and render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

	//Disable vertex position
	glDisableVertexAttribArray(gVertexPos2DLocation);

	//Unbind program
	glUseProgram(NULL);

	SDL_GL_SwapWindow(m_Window);
}

PlatformAccess::PlatformAccess(const DependencyInjection::ConfigurationProvider* configs)
	: m_Configs(configs)
{
	// initialize SDL
	if (!InitializeSDL())
		throw std::runtime_error(s_SDLInitializationError);

	//Initialize OpenGL
	if (!InitializeGL())
		throw std::runtime_error(s_GLInitializationError);
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

void Engine::Core::Platform::PlatformAccess::TempUpdate()
{
	Render();
}
