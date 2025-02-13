#pragma once

#include "scriptable_type.h"

#include <vector>
#include <unordered_map>


// Begin defining a reflection-enabled type; to avoid string-name collision, prefer to use FULLY-QUALIFIED names!
#define SE_REFLECTION_Begin(type) \
template <>\
const SigourneyEngine::Core::Reflection::ScriptableType* SigourneyEngine::Core::Reflection::GetType<type>() \
{\
	using OwnerType = type;\
	static SE_REFLECTION_SUBSPACE::ReflectionBuildingContext context{{ #type }}; \
	static SigourneyEngine::Core::Reflection::ScriptableType* ptr = context\

// Adds a named property to the current registration
#define SE_REFLECTION_AddProperty(member) AddProperty({ #member, SE_REFLECTION_SUBSPACE::GetMemberType(&OwnerType::member), offsetof(OwnerType, member) })

// Wrap up a reflection type definition.
#define SE_REFLECTION_End \
		Finalize();\
	return ptr;\
}


namespace SE_REFLECTION_SUBSPACE {

struct ReflectionBuildingContext
{
	SigourneyEngine::Core::Reflection::ScriptableType Type;

	inline ReflectionBuildingContext& AddProperty(const SigourneyEngine::Core::Reflection::ScriptableProperty&& nextProperty)
	{
		Type.Properties.push_back(nextProperty);
		return *this;
	}

	SigourneyEngine::Core::Reflection::ScriptableType* Finalize();
};

template <typename TOwner, typename TMember>
SigourneyEngine::Core::Reflection::DataType GetMemberType(TMember TOwner::* memberPtr)
{
	return SigourneyEngine::Core::Reflection::GetTypeInner<TMember>();
}

}


namespace SigourneyEngine {
namespace Core {
namespace Reflection {


template <typename T>
const ScriptableType* GetType();

const ScriptableType* GetType(std::string name);

}
}
}
