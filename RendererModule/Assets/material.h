#pragma once

#include "shader.h"

#include <Reflection/reference_type.h>

namespace Engine {
namespace RendererModule {
namespace Assets {

class Material
{
private:
	unsigned int m_RendererID = 0;

public:
	Core::Reflection::Ref<VertexShader> VertexShader;
	Core::Reflection::Ref<FragmentShader> FragmentShader;

	static void Initialize(Core::DependencyInjection::ServiceProvider* services, void* asset);
	static void Dispose(Core::DependencyInjection::ServiceProvider* services, void* asset);
};

}
}
}
