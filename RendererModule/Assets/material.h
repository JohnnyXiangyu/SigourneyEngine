#pragma once

#include "shader.h"

#include <Rendering/renderer_data.h>
#include <Reflection/reference_type.h>

namespace Extension {
namespace RendererModule {

class RendererModule;

namespace Assets {

class Material
{
private:
	friend class Extension::RendererModule::RendererModule;
	Engine::Core::Rendering::RendererMaterial m_RendererID;

public:
	Engine::Core::Reflection::Ref<VertexShader> VertexShader;
	Engine::Core::Reflection::Ref<FragmentShader> FragmentShader;

	static void Initialize(Engine::Core::DependencyInjection::ServiceProvider* services, void* asset);
	static void Dispose(Engine::Core::DependencyInjection::ServiceProvider* services, void* asset);
};

}
}
}
