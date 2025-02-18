#pragma once

#include <GL/glew.h>

namespace Engine {
namespace Core {
namespace Rendering {

enum class ShaderType
{
	FRAGMENT_SHADER,
	VERTEX_SHADER
};

enum class GpuDataType
{
	INT32 = GL_INT,
	UINT32 = GL_UNSIGNED_INT,
	FLOAT = GL_FLOAT,
	DOUBLE = GL_DOUBLE,
	BYTE = GL_BYTE
};

struct VertexBufferLayoutElement
{
	GpuDataType Type;
	unsigned int Count;
	bool Normalized;
};

struct VertexCollection
{
	const void* Vertices;
	unsigned int Length;
};

struct VertexLayout
{
	VertexBufferLayoutElement* Elements;
	unsigned int Length;
};

struct IndexCollection
{
	const unsigned int* Indices;
	unsigned int Length;
};


enum class ShaderParamType
{
	INT32,
	UINT32,
	FLOAT,
	DOUBLE,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4
};

/// <summary>
/// Denote parameters passed in during QueueRender
/// </summary>
struct DynamicShaderParameter
{
	const std::string& Name;
	void* Data;
	ShaderParamType Type;
};

}
}
}
