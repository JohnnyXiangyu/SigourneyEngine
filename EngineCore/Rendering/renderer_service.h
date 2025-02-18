#pragma once

#include "Utils/non_essential_macros.h"
#include "Logging/logger_service.h"
#include "renderer_data.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>


namespace Engine {
namespace Core {
namespace Rendering {


class RendererShader
{
private:
	friend class RendererService;
	unsigned int m_ShaderID = 0;
};


class RendererMaterial
{
private:
	friend class RendererService;
	unsigned int m_ProgramID = 0;
	std::unordered_map<std::string, int> m_UniformLocationCache;
};


/// <summary>
/// Encapsulates an internal representation of data structures and contracts between the renderer service and graphics backend.
/// Currently it's just VB, VA and IB tokens from OpenGL.
/// </summary>
class RendererMesh
{
private:
	friend class RendererService;

	// these are buffers
	unsigned int m_VertexBuffer = 0;
	unsigned int m_IndexBuffer = 0;

	// metadata
	unsigned int m_IndexCount = 0;

	// this is not a buffer
	unsigned int m_VertexArray = 0;

	bool Bail();
};


/// <summary>
/// This iteration only uses small boxes;
/// Each has two integers for its dimensions and two integers for its position, and another three integers for its color.
/// TODO: add a render texture in here (texture loading requires a separate asset loading service)
/// </summary>
class RendererService
{
private:
	// TODO: if we are not running multi-threaded rendering there's no need for a queue; if it is multithreaded we'll use a concurrent queue for that
	//// generic form command that client code submit to the queue; all data is owned by the client and the queue doesn't check for validity
	//struct RenderCommand
	//{
	//	const RendererMesh* Mesh; 
	//	const RendererMaterial* Material;
	//	const DynamicShaderParameter* ShaderParams;
	//	unsigned int ShaderParamCount;
	//};
	//std::vector<RenderCommand> m_RenderCommandQueue;

private:
	Logging::LoggerService* m_Logger = Logging::GetLogger();

private:
	bool CheckGLError(const char* function, const char* file, int line);
	bool ApplyDynamicShaderParameter(const Rendering::DynamicShaderParameter* shaderParam, Rendering::RendererMaterial* material);

public:
	RendererService();
	~RendererService();

public:
	bool CompileShader(const std::string& code, ShaderType type, RendererShader& outID);
	bool DeleteShader(RendererShader& shader);

	bool CreateMaterial(const RendererShader& vertexShader, const RendererShader& fragmentShader, RendererMaterial& outID);
	bool DeleteMaterial(RendererMaterial& material);

	bool RegisterMesh(const VertexCollection& vertices, const VertexLayout& vertexLayout, const IndexCollection& indices, RendererMesh& outID);
	bool DeleteMesh(RendererMesh& inID);

	bool QueueRender(RendererMesh* mesh, RendererMaterial* material, const DynamicShaderParameter* shaderParams, unsigned int shaderParamCount);
};

}
}
}
