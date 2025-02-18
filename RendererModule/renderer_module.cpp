#include "pch.h"
#include "renderer_module.h"
#include "Assets/mesh.h"
#include "Assets/material.h"

#include <string>

using namespace Engine::Core;
using namespace Extension::RendererModule;
using namespace Extension::RendererModule::Assets;


static std::string s_HardCodeUniformName("u_MVP");


std::string RendererModule::Name("RendererModule");


void* Extension::RendererModule::RendererModule::Create(Engine::Core::DependencyInjection::ServiceProvider* services)
{
    // temp: load a mesh and a material
    RendererModule* newModule = services->GetGlobalAllocator()->New<RendererModule>();

    newModule->m_Mesh = services->GetAssetManager()->LoadAsset<Assets::Mesh>("Meshes/cube.se_mesh");
    newModule->m_Material = services->GetAssetManager()->LoadAsset<Assets::Material>("Materials/dummy.se_material");

    return newModule;
}

void Extension::RendererModule::RendererModule::Update(void* moduleInstance, Engine::Core::DependencyInjection::ServiceProvider* services)
{
    RendererModule* instance = (RendererModule*)moduleInstance;

    instance->m_ProjectionMatrix = glm::perspective(glm::radians<float>(75), 960.0f / 720.0f, 0.1f, 10000.0f);
    instance->m_View = glm::translate(glm::mat4(1.0f), glm::vec3(100, 100, -500));
    instance->m_Model = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 100)), glm::radians<float>(45), glm::vec3(0, 1, 0));

    // calculate MVP
    glm::mat4 mvp = instance->m_ProjectionMatrix * instance->m_View * instance->m_Model;

    // temp: create parameters locally
    Rendering::DynamicShaderParameter newParams{ s_HardCodeUniformName, &mvp, Rendering::ShaderParamType::MAT4 };

    // submit value to the renderer
    services->GetRenderer()->QueueRender(&instance->m_Mesh->m_RendererCopy, &instance->m_Material->m_RendererID, &newParams, 1);
}

void Extension::RendererModule::RendererModule::Finalize(void* moduleInstance, Engine::Core::DependencyInjection::ServiceProvider* services)
{
    // no need to delete the assets, they are managed currently by the asset manager
}
