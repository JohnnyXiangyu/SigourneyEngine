#include "pch.h"
#include "material.h"

#include <Reflection/type_info_registra.h>
#include <DependencyInjection/service_provider.h>

using namespace Engine;
using namespace Engine::RendererModule;


void Assets::Material::Initialize(Core::DependencyInjection::ServiceProvider* services, void* asset)
{
	Assets::Material* material = (Assets::Material*)asset;
	if (!services->GetRenderer()->LinkShaderProgram(material->VertexShader.Data->RendererID, material->FragmentShader.Data->RendererID, material->m_RendererID))
	{
		SE_THROW_GRAPHICS_EXCEPTION;
	}
}


void Engine::RendererModule::Assets::Material::Dispose(Core::DependencyInjection::ServiceProvider* services, void* asset)
{
	Assets::Material* material = (Assets::Material*)asset;
	services->GetRenderer()->DeleteProgram(material->m_RendererID);
}


SE_REFLECTION_BEGIN(Assets::Material)
.SE_REFLECTION_ADD_REFERENCE(VertexShader)
.SE_REFLECTION_ADD_REFERENCE(FragmentShader)
.SE_REFLECTION_DELETE_SERIALIZER()
.SE_REFLECTION_OVERRIDE_INITIALIZER(Assets::Material::Initialize)
.SE_REFLECTION_OVERRIDE_DISPOSER(Assets::Material::Dispose)
.SE_REFLECTION_END
