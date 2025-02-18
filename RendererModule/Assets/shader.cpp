#include "pch.h"
#include "shader.h"

using namespace Engine;
using namespace Extension::RendererModule;

static void* DeserializeVertexShader(Core::DependencyInjection::ServiceProvider* services, Core::AssetManagement::ByteStream* source)
{
	// load the entire string in
	char readBuffer[Core::Configuration::STRING_LOAD_BUFFER_SIZE];
	std::string rawData;
	rawData.reserve(Core::Configuration::STRING_LOAD_BUFFER_SIZE);
	long long newReads = 0;
	while ((newReads = source->Read(readBuffer, Core::Configuration::STRING_LOAD_BUFFER_SIZE - 1)) > 0)
	{
		readBuffer[newReads] = 0;
		rawData.append(readBuffer);
	}

	// submit shader code to the rendering service
	Core::Rendering::RendererShader rendererID;
	if (!services->GetRenderer()->CompileShader(rawData, Core::Rendering::ShaderType::VERTEX_SHADER, rendererID))
	{
		return nullptr;
	}

	// allocate new shader data structure and return it
	return services->GetGlobalAllocator()->New<Assets::VertexShader>({ rendererID });
}


static void DisposeVertexShader(Core::DependencyInjection::ServiceProvider* services, void* data)
{
	Assets::VertexShader* shader = (Assets::VertexShader*)data;
	services->GetRenderer()->DeleteShader(shader->RendererID);
	services->GetGlobalAllocator()->Free(data);
}


SE_REFLECTION_BEGIN(Extension::RendererModule::Assets::VertexShader)
.SE_REFLECTION_OVERRIDE_DESERIALIZER(DeserializeVertexShader)
.SE_REFLECTION_DELETE_SERIALIZER()
.SE_REFLECTION_OVERRIDE_DISPOSER(DisposeVertexShader)
.SE_REFLECTION_END


static void* DeserializeFragmentShader(Core::DependencyInjection::ServiceProvider* services, Core::AssetManagement::ByteStream* source)
{
	// load the entire string in
	std::string rawData;
	rawData.reserve(Core::Configuration::STRING_LOAD_BUFFER_SIZE);
	source->ReadAllAsString(rawData);

	// TODO: submit this to the rendering engine
	Core::Rendering::RendererShader rendererID;
	if (!services->GetRenderer()->CompileShader(rawData, Core::Rendering::ShaderType::FRAGMENT_SHADER, rendererID))
	{
		// TODO: this will crash the game on scene load, might not be the best solution?
		SE_THROW_GRAPHICS_EXCEPTION;
	}

	// allocate new shader data structure and return it
	return services->GetGlobalAllocator()->New<Assets::FragmentShader>({ rendererID });
}


static void DisposeFragmentShader(Core::DependencyInjection::ServiceProvider* services, void* data)
{
	Assets::FragmentShader* shader = (Assets::FragmentShader*)data;
	services->GetRenderer()->DeleteShader(shader->RendererID);
	services->GetGlobalAllocator()->Free(data);
}


SE_REFLECTION_BEGIN(Extension::RendererModule::Assets::FragmentShader)
.SE_REFLECTION_OVERRIDE_DESERIALIZER(DeserializeFragmentShader)
.SE_REFLECTION_DELETE_SERIALIZER()
.SE_REFLECTION_OVERRIDE_DISPOSER(DisposeFragmentShader)
.SE_REFLECTION_END
