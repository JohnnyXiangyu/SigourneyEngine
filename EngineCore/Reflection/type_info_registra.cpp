#include "type_info_registra.h"

#include "Logging/logger_service.h"

using namespace SigourneyEngine::Core;
using namespace SigourneyEngine::Core::Reflection;

static std::unordered_map<std::string, SigourneyEngine::Core::Reflection::ScriptableType*> s_ReflectionMap;

SigourneyEngine::Core::Reflection::ScriptableType* SE_REFLECTION_SUBSPACE::ReflectionBuildingContext::Finalize()
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

const ScriptableType* SigourneyEngine::Core::Reflection::GetType(std::string name)
{
	auto found = s_ReflectionMap.find(name);
	if (found == s_ReflectionMap.end())
		return nullptr;
	return found->second;
}
