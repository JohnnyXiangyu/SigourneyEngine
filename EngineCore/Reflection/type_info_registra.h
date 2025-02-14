#pragma once

#include "scriptable_type.h"
#include "reference_type.h"
#include "reflection.h"

#include <vector>
#include <unordered_map>


// Begin defining a reflection-enabled type; to avoid string-name collision, prefer to use FULLY-QUALIFIED names!
#define SE_REFLECTION_BEGIN(type) \
template <>\
const Engine::Core::Reflection::ScriptableType* Engine::Core::Reflection::GetType<type>() \
{\
	using OwnerType = type;\
	struct __Local_Method_Holder \
	{\
		static void* __GetCDO(void* ptr) \
		{\
			return new (ptr) OwnerType();\
		}\
	};\
	static SE_REFLECTION_SUBSPACE::ReflectionBuildingContext context{{ #type, sizeof(type), __Local_Method_Holder::__GetCDO }}; \
	static Engine::Core::Reflection::ScriptableType* ptr = context

// Adds a named data property (simple data type) to the current registration
#define SE_REFLECTION_ADD_PROPERTY(member) AddProperty({ #member, SE_REFLECTION_SUBSPACE::GetDataMemberType(&OwnerType::member), offsetof(OwnerType, member) })

// Adds a named asset reference property
#define SE_REFLECTION_ADD_REFERENCE(member) AddReference( { #member, SE_REFLECTION_SUBSPACE::GetRefMemberType(&OwnerType::member), offsetof(OwnerType, member) } )

// Override the default key-value based serializer; this allows you to freely ma
#define SE_REFLECTION_OVERRIDE_SERIALIZER(factory, disposal) OverrideSerializer( { factory, disposal } )

// Wrap up a reflection type definition.
#define SE_REFLECTION_END \
		Finalize();\
	return ptr;\
}


namespace SE_REFLECTION_SUBSPACE {

struct ReflectionBuildingContext
{
	Engine::Core::Reflection::ScriptableType Type;

	inline ReflectionBuildingContext& AddProperty(const Engine::Core::Reflection::ScriptableProperty&& nextProperty)
	{
		Type.Properties.push_back(nextProperty);
		return *this;
	}

	inline ReflectionBuildingContext& AddReference(const Engine::Core::Reflection::ReferenceProperty&& nextProperty)
	{
		Type.References.push_back(nextProperty);
		return *this;
	}

	inline ReflectionBuildingContext& OverrideSerializer(const Engine::Core::Reflection::Serializer&& newSerializer)
	{
		Type.SerializerOverride = newSerializer;
		return *this;
	}

	Engine::Core::Reflection::ScriptableType* Finalize();
};

template <typename T>
Engine::Core::Reflection::DataType GetTypeInner();

template <typename TOwner, typename TMember>
Engine::Core::Reflection::DataType GetDataMemberType(TMember TOwner::* memberPtr)
{
	return SE_REFLECTION_SUBSPACE::GetTypeInner<TMember>();
}

template <typename TOwner, typename TMember>
const Engine::Core::Reflection::ScriptableType* GetRefMemberType(Engine::Core::Reflection::Ref<TMember> TOwner::* memberPtr)
{
	return Engine::Core::Reflection::GetType<TMember>();
}

}
