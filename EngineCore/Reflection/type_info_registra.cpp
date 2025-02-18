#include "type_info_registra.h"

#include "Logging/logger_service.h"

#include <unordered_map>

using namespace Engine::Core;
using namespace Engine::Core::Reflection;

static std::unordered_map<std::string, Engine::Core::Reflection::ScriptableType*> s_ReflectionMap;

Engine::Core::Reflection::ScriptableType* SE_REFLECTION_SUBSPACE::ReflectionBuildingContext::Finalize()
{
	auto found = s_ReflectionMap.find(Type.Name);
	if (found != s_ReflectionMap.end())
	{
		Logging::GetLogger()->Warning("Reflection", "Duplicate reflection type key: %s, did you fully qualify the typename?", Type.Name.c_str());
		return found->second;
	}

	s_ReflectionMap.insert(std::make_pair(Type.Name, &Type));
	return &Type;
}

const ScriptableType* Engine::Core::Reflection::GetType(std::string name)
{
	auto found = s_ReflectionMap.find(name);
	if (found == s_ReflectionMap.end())
		return nullptr;
	return found->second;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<bool>()
{
	return DataType::BOOL;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<int>()
{
	return DataType::INT32;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<unsigned int>()
{
	return DataType::UINT32;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<long long>()
{
	return DataType::INT64;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<unsigned long long>()
{
	return DataType::UINT64;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<float>()
{
	return DataType::FLOAT;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<double>()
{
	return DataType::DOUBLE;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<glm::vec2>()
{
	return DataType::VEC2;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<glm::vec3>()
{
	return DataType::VEC3;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<glm::vec4>()
{
	return DataType::VEC4;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<glm::mat2>()
{
	return DataType::MAT2;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<glm::mat3>()
{
	return DataType::MAT3;
}

template <>
DataType SE_REFLECTION_SUBSPACE::GetTypeInner<glm::mat4>()
{
	return DataType::MAT4;
}

void* SE_REFLECTION_SUBSPACE::DeleteDeserializer(Engine::Core::DependencyInjection::ServiceProvider* services, Engine::Core::AssetManagement::ByteStream* source)
{
	SE_THROW_NOT_IMPLEMENTED;
}

void SE_REFLECTION_SUBSPACE::DeleteIntializer(Engine::Core::DependencyInjection::ServiceProvider* services, void* asset)
{
	SE_THROW_NOT_IMPLEMENTED;
}

void SE_REFLECTION_SUBSPACE::DeleteSerialize(Engine::Core::DependencyInjection::ServiceProvider* services, Engine::Core::AssetManagement::ByteStream* destination)
{
	SE_THROW_NOT_IMPLEMENTED;
}

void SE_REFLECTION_SUBSPACE::DeleteDisposal(Engine::Core::DependencyInjection::ServiceProvider* services, void* data)
{
	SE_THROW_NOT_IMPLEMENTED;
}
