#include "renderer_service.h"
#include "ErrorHandling/exceptions.h"

#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#define GLCallLite(routine) ClearGLErrors();\
    routine;\
    return CheckGLError(#routine, __FILE__, __LINE__)

#define GLCall(routine) ClearGLErrors();\
    routine;\
    if (!CheckGLError(#routine, __FILE__, __LINE__))\
        goto OnError


using namespace Engine;
using namespace Engine::Core;


static const char s_ChannelName[] = "RendererService";


static void ClearGLErrors()
{
    while (glGetError() != GL_NO_ERROR);
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


static unsigned int GetGpuDataSize(Rendering::GpuDataType type)
{
    switch (type)
    {
    case Rendering::GpuDataType::INT32:
        return sizeof(GLint);
    case Rendering::GpuDataType::UINT32:
        return sizeof(GLuint);
    case Rendering::GpuDataType::FLOAT:
        return sizeof(GLfloat);
    case Rendering::GpuDataType::DOUBLE:
        return sizeof(GLdouble);
    case Rendering::GpuDataType::BYTE:
        return sizeof(GLbyte);
    default:
        return 0;
    }
}


static unsigned int GetStride(const Rendering::VertexLayout& layout)
{
    unsigned int stride = 0;
    for (unsigned int i = 0; i < layout.Length; i++)
    {
        stride += layout.Elements[i].Count * GetGpuDataSize(layout.Elements[i].Type);
    }
    return stride;
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

Engine::Core::Rendering::RendererService::RendererService()
{
    // TODO: load configurations
}

Engine::Core::Rendering::RendererService::~RendererService()
{
}


bool Engine::Core::Rendering::RendererService::CompileShader(const std::string& code, ShaderType type, RendererShader& outID)
{
    {
        unsigned int glShaderType = TranslateShaderType(type);
        const char* src = code.c_str();

        GLCall(outID.m_ShaderID = glCreateShader(glShaderType));
        GLCall(glShaderSource(outID.m_ShaderID, 1, &src, nullptr));
        GLCall(glCompileShader(outID.m_ShaderID));

        int result;
        GLCall(glGetShaderiv(outID.m_ShaderID, GL_COMPILE_STATUS, &result));

        // handle compilation error (no crash)
        if (!result)
        {
            int length;
            glGetShaderiv(outID.m_ShaderID, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*)_malloca(length * sizeof(char));
            glGetShaderInfoLog(outID.m_ShaderID, length, &length, message);
            m_Logger->Error(s_ChannelName, "Failed to compile shader %s, error: %s", PrintShaderType(type), message);
            glDeleteShader(outID.m_ShaderID);

            ClearGLErrors();
            return false;
        }

        return true;
    }

OnError:
    if (outID.m_ShaderID != 0)
    {
        glDeleteShader(outID.m_ShaderID);
        outID.m_ShaderID = 0;
        ClearGLErrors();
    }
    return false;
}


bool Engine::Core::Rendering::RendererService::DeleteShader(RendererShader& shader)
{
    GLCallLite(glDeleteShader(shader.m_ShaderID));
}


bool Engine::Core::Rendering::RendererService::CreateMaterial(const RendererShader& vertexShader, const RendererShader& fragmentShader, RendererMaterial& outID)
{
    GLCall(outID.m_ProgramID = glCreateProgram());
    GLCall(glAttachShader(outID.m_ProgramID, vertexShader.m_ShaderID));
    GLCall(glAttachShader(outID.m_ProgramID, fragmentShader.m_ShaderID));
    GLCall(glLinkProgram(outID.m_ProgramID));
    return true;

OnError:
    if (outID.m_ProgramID != 0)
    {
        glDeleteProgram(outID.m_ProgramID);
        ClearGLErrors();
    }
    return false;
}


bool Engine::Core::Rendering::RendererService::DeleteMaterial(RendererMaterial& material)
{
    GLCallLite(glDeleteProgram(material.m_ProgramID));
}


bool Engine::Core::Rendering::RendererService::RegisterMesh(const VertexCollection& vertices, const VertexLayout& vertexLayout, const IndexCollection& indices, RendererMesh& outID)
{
    // batch create the buffers (2)
    GLCall(glGenBuffers(2, (unsigned int*) & outID));
    
    // creat the vertex array
    GLCall(glGenVertexArrays(1, &outID.m_VertexArray));

    // load the buffers
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, outID.m_VertexBuffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.Length, vertices.Vertices, GL_STATIC_DRAW));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outID.m_IndexBuffer));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.Length * sizeof(unsigned int), indices.Indices, GL_STATIC_DRAW));
    outID.m_IndexCount = indices.Length;

    // populate our index array
    GLCall(glBindVertexArray(outID.m_VertexArray));
    GLCall(glEnableVertexAttribArray(0));
    {
        unsigned int layoutStride = GetStride(vertexLayout);
        unsigned long long offset = 0;
        for (unsigned int i = 0; i < vertexLayout.Length; i++)
        {
            GLCall(glEnableVertexAttribArray(i));
            glVertexAttribPointer(i,
                vertexLayout.Elements[i].Count,
                (unsigned int)vertexLayout.Elements[i].Type,
                vertexLayout.Elements[i].Normalized ? GL_TRUE : GL_FALSE,
                layoutStride,
                (const void*)offset);
            offset += (unsigned long long)vertexLayout.Elements[i].Count * GetGpuDataSize(vertexLayout.Elements[i].Type);
        }
    }
    return true;

OnError:
    return outID.Bail();
}


bool Engine::Core::Rendering::RendererService::DeleteMesh(RendererMesh& inID)
{
    if (inID.m_VertexArray != 0)
    {
        GLCallLite(glDeleteVertexArrays(1, &inID.m_VertexArray));
    }

    if (inID.m_IndexBuffer != 0 || inID.m_VertexBuffer != 0)
    {
        GLCallLite(glDeleteBuffers(2, (unsigned int*)&inID));
    }

    inID.m_IndexBuffer = 0;
    inID.m_VertexArray = 0;
    inID.m_VertexBuffer = 0;

    return true;
}


bool Rendering::RendererService::ApplyDynamicShaderParameter(const Rendering::DynamicShaderParameter* shaderParam, Rendering::RendererMaterial* material)
{
    // TODO: implement some kind of parameter hint system so renderer can pre-cache the locations
    int location = -1;
    auto foundInCache = material->m_UniformLocationCache.find(shaderParam->Name);
    if (foundInCache != material->m_UniformLocationCache.end())
    {
        location = foundInCache->second;
    }
    else
    {
        // assume shader program is already bound here
        GLCall(location = glGetUniformLocation(material->m_ProgramID, shaderParam->Name.c_str()));
        material->m_UniformLocationCache.insert(std::make_pair(shaderParam->Name, location));
    }

    switch (shaderParam->Type)
    {
    case ShaderParamType::INT32:
        GLCall(glUniform1i(location, *static_cast<int*>(shaderParam->Data)));
        break;
    case ShaderParamType::UINT32:
        GLCall(glUniform1ui(location, *static_cast<unsigned int*>(shaderParam->Data)));
        break;
    case ShaderParamType::FLOAT:
        GLCall(glUniform1f(location, *static_cast<float*>(shaderParam->Data)));
        break;
    case ShaderParamType::DOUBLE:
        GLCall(glUniform1d(location, *static_cast<double*>(shaderParam->Data)));
        break;
    case ShaderParamType::VEC2:
        GLCall(glUniform2fv(location, 1, &(*static_cast<glm::vec2*>(shaderParam->Data))[0]));
        break;
    case ShaderParamType::VEC3:
        GLCall(glUniform3fv(location, 1, &(*static_cast<glm::vec3*>(shaderParam->Data))[0]));
        break;
    case ShaderParamType::VEC4:
        GLCall(glUniform4fv(location, 1, &(*static_cast<glm::vec4*>(shaderParam->Data))[0]));
        break;
    case ShaderParamType::MAT2:
        GLCall(glUniformMatrix2fv(location, 1, GL_FALSE, &(*static_cast<glm::mat2*>(shaderParam->Data))[0][0]));
        break;
    case ShaderParamType::MAT3:
        GLCall(glUniformMatrix3fv(location, 1, GL_FALSE, &(*static_cast<glm::mat3*>(shaderParam->Data))[0][0]));
        break;
    case ShaderParamType::MAT4:
        GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &(*static_cast<glm::mat4*>(shaderParam->Data))[0][0]));
        break;
    }

    return true;

OnError:
    return false;
}


bool Rendering::RendererService::QueueRender(RendererMesh* mesh, RendererMaterial* material, const Rendering::DynamicShaderParameter* shaderParams, unsigned int shaderParamCount)
{
    // render the object with opengl
    GLCall(glUseProgram(material->m_ProgramID));
    GLCall(glBindVertexArray(mesh->m_VertexArray));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_IndexBuffer));

    // set parameters to the shader
    for (unsigned int i = 0; i < shaderParamCount; i++)
    {
        ApplyDynamicShaderParameter(shaderParams + i, material);
    }

    // draw call TODO: how does batch work?
    GLCall(glDrawElements(GL_TRIANGLES, mesh->m_IndexCount, GL_UNSIGNED_INT, nullptr));
    return true;

OnError:
    // leave the logging and stuff to user, at this stage failed GL operations would mean very little to the overall integrity of the program
    return false;
}


bool Engine::Core::Rendering::RendererMesh::Bail()
{
    // note: since this is a bail function, we don't check errors and just let them pass
    if (m_VertexArray != 0)
    {
        glDeleteVertexArrays(1, &m_VertexArray);
    }

    if (m_IndexBuffer != 0 || m_VertexBuffer != 0)
    {
        glDeleteBuffers(2, (unsigned int*)this);
    }

    ClearGLErrors();

    m_VertexArray = 0;
    m_VertexArray = 0;
    m_IndexBuffer = 0;

    return false;
}
