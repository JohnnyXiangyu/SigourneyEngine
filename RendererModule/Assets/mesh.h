#pragma once

#include <DependencyInjection/service_provider.h>
#include <AssetManagement/byte_stream.h>
#include <Rendering/renderer_data.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>


namespace Extension {
namespace RendererModule {

class RendererModule;

namespace Assets {

struct Vertex
{
	glm::vec3 position{};
	glm::vec3 normal{};
	glm::vec2 uv{};
};

/// <summary>
/// Mesh = vertex array + index buffer, vertex array = vertex buffer + layout
/// </summary>
class Mesh
{
private:
	friend class Extension::RendererModule::RendererModule;
	Engine::Core::Rendering::RendererMesh m_RendererCopy;

public:
	static void* LoadMesh(Engine::Core::DependencyInjection::ServiceProvider* services, Engine::Core::AssetManagement::ByteStream* source);
	static void DisposeMesh(Engine::Core::DependencyInjection::ServiceProvider* services, void* data);
};

}
}
}
