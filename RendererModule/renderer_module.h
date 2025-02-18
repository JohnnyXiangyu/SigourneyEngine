#pragma once

#include <string>
#include <DependencyInjection/service_provider.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Extension {
namespace RendererModule {

namespace Assets {

class Material;
class Mesh;

}

class RendererModule
{
private:
	// temp: need to construct a list of these from entities; this is essentially demo for a singular object
	// mvp construction should be managed by the module
	Assets::Material* m_Material;
	Assets::Mesh* m_Mesh;

	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_View;
	glm::mat4 m_Model;

public:
	static void* Create(Engine::Core::DependencyInjection::ServiceProvider* services);
	static void Update(void* moduleInstance, Engine::Core::DependencyInjection::ServiceProvider* services);
	static void Finalize(void* moduleInstance, Engine::Core::DependencyInjection::ServiceProvider* services);
	static std::string Name;
};

}
}
