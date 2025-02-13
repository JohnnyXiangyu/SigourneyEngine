#pragma once

#include "scriptable_type.h"
#include "data_type.h"

#include "Logging/logger_service.h"

#include <unordered_map>
#include <string>

// pretend this is a member function of ReflectionManager, starts a registration that needs to be terminated by a SE_REFLECTION_Finalize
#define SE_REFLECTION_BeginTypeDefinition(type) __RegisterReflection(#type, [](SigourneyEngine::Core::Reflection::ScriptableType* result) { using __Current_Reflected = type; SigourneyEngine::Core::Reflection::ReflectionBuildingContext{result}

// pretend this is a member function of ReflectionManager, adds a named property to the current registration
#define SE_REFLECTION_AddProperty(member) Add({ #member, SigourneyEngine::Core::Reflection::GetMemberType(&__Current_Reflected::member), offsetof(__Current_Reflected, member) })

// finishes the current registration
#define SE_REFLECTION_Finalize() __Finalize(); })

namespace SigourneyEngine {
namespace Core {
namespace Reflection {


template <typename TOwner, typename TMember>
DataType GetMemberType(TMember TOwner::* memberPtr) 
{
	return GetTypeInner<TMember>();
}


class ReflectionManager
{
private:
	Logging::LoggerService* m_Logger;

private:
	std::unordered_map<std::string, ScriptableType> m_ReflectionTypes;

public:
	ReflectionManager(Logging::LoggerService* logger) : m_Logger(logger) {}

	// type registration
public:
	template <typename F>
	void __RegisterReflection(const std::string& name, F&& factory)
	{
		if (m_ReflectionTypes.find(name) != m_ReflectionTypes.end())
		{
			m_Logger->Warning("ReflectionManager", "Duplicated type registration: %s", name.c_str());
			return;
		}

		m_ReflectionTypes.insert(std::make_pair(name, ScriptableType{ name }));
		factory(&m_ReflectionTypes[name]);
	}

	const ScriptableType* GetType(const std::string& name);
};


struct ReflectionBuildingContext
{
	ScriptableType* Type;

	inline ReflectionBuildingContext& Add(const ScriptableProperty&& nextProperty)
	{
		Type->Properties.push_back(nextProperty);
		return *this;
	}

	inline void __Finalize() {}
};


}
}
}
