#include "renderer_service.h"
#include "ErrorHandling/exceptions.h"

#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string>

// the error handling strategy here is to bubble the error up to the caller
#define GLCall(routine) ClearGLErrors();\
    routine;\
    if (!CheckGLError(#routine, __FILE__, __LINE__)) \
        return false\

using namespace Engine;
using namespace Engine::Core;


static const char s_ChannelName[] = "RendererService";


static void ClearGLErrors()
{
    while (glGetError() != GL_NO_ERROR);
}


bool Rendering::RendererService::CheckGLError(const char* function, const char* file, int line)
{
    bool hasError = false;
    for (GLenum errorCode = glGetError(); errorCode != GL_NO_ERROR; errorCode = glGetError())
    {
        m_Logger->Error(s_ChannelName, "OPENGL Error #%d at %s:%d, %s", errorCode, file, line, function);
        hasError = true;
    }

    return !hasError;
}


static unsigned int TranslateShaderType(Rendering::ShaderType type)
{
    switch (type)
    {
    case Rendering::ShaderType::FRAGMENT_SHADER:
        return GL_FRAGMENT_SHADER;
    case Rendering::ShaderType::VERTEX_SHADER:
        return GL_VERTEX_SHADER;
    default:
        SE_THROW_NOT_IMPLEMENTED;
    }
}


static const char* PrintShaderType(Rendering::ShaderType type)
{
    static const char s_ShaderTypeFrag[] = "FRAGMENT";
    static const char s_ShaderTypeVert[] = "VERTEX";

    switch (type)
    {
    case Rendering::ShaderType::FRAGMENT_SHADER:
        return s_ShaderTypeFrag;
    case Rendering::ShaderType::VERTEX_SHADER:
        return s_ShaderTypeVert;
    default:
        SE_THROW_NOT_IMPLEMENTED;
    }
}


bool Engine::Core::Rendering::RendererService::CompileShader(const std::string& source, unsigned int& outID, ShaderType type)
{
    unsigned int glShaderType = TranslateShaderType(type);

    GLCall(outID = glCreateShader(glShaderType));
    const char* src = source.c_str();

    GLCall(glShaderSource(outID, 1, &src, nullptr));
    GLCall(glCompileShader(outID));

    int result;
    GLCall(glGetShaderiv(outID, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        // log shader error
        int length;
        GLCall(glGetShaderiv(outID, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)_malloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(outID, length, &length, message));
        m_Logger->Error(s_ChannelName, "Failed to compile shader %s, error: %s", PrintShaderType(type), message);

        // clean up
        GLCall(glDeleteShader(outID));
        return false;
    }

    return true;
}


bool Engine::Core::Rendering::RendererService::LinkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader, unsigned int& outID)
{
    GLCall(outID = glCreateProgram());

    GLCall(glAttachShader(outID, vertexShader));
    GLCall(glAttachShader(outID, fragmentShader));

    GLCall(glLinkProgram(outID));

    return true;
}


bool Engine::Core::Rendering::RendererService::DeleteShader(unsigned int shader)
{
    GLCall(glDeleteShader(shader));
    return true;
}

bool Engine::Core::Rendering::RendererService::DeleteProgram(unsigned int program)
{
    GLCall(glDeleteProgram(program));
    return true;
}
