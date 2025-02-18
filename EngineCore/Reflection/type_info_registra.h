#pragma once

#include "scriptable_type.h"
#include "reference_type.h"
#include "reflection.h"

#include <vector>


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

#define SE_REFLECTION_CUSTOMCDO_BEGIN(type, customCDO)  \
template <>\
const Engine::Core::Reflection::ScriptableType* Engine::Core::Reflection::GetType<type>() \
{\
	using OwnerType = type;\
	struct __Local_Method_Holder \
	{\
		static void* __GetCDO(void* ptr) \
		{\
			return new (ptr) customCDO;\
		}\
	};\
	static SE_REFLECTION_SUBSPACE::ReflectionBuildingContext context{{ #type, sizeof(type), __Local_Method_Holder::__GetCDO }}; \
	static Engine::Core::Reflection::ScriptableType* ptr = context

// Adds a named data property (simple data type) to the current registration (needs to be public member)
#define SE_REFLECTION_ADD_PROPERTY(member) \
AddProperty({ #member, SE_REFLECTION_SUBSPACE::GetDataMemberType(&OwnerType::member), offsetof(OwnerType, member) })

// Adds a named asset reference property (needs to be public member)
#define SE_REFLECTION_ADD_REFERENCE(member) \
AddReference( { #member, SE_REFLECTION_SUBSPACE::GetRefMemberType(&OwnerType::member), offsetof(OwnerType, member) } )

// TODO: support adding data types that are other reflected types??

#define SE_REFLECTION_OVERRIDE_DESERIALIZER(deserialize) \
OverrideDeserializer(deserialize)

#define SE_REFLECTION_DELETE_DESERIALIZER() \
OverrideDeserializer(SE_REFLECTION_SUBSPACE::DeleteDeserializer)

#define SE_REFLECTION_OVERRIDE_INITIALIZER(initialize) \
OverrideInitializer(initialize)

#define SE_REFLECTION_DELETE_INITIALIZER() \
OverrideInitializer(SE_REFLECTION_SUBSPACE::DeleteIntializer)

#define SE_REFLECTION_OVERRIDE_SERIALIZER(serialize) \
OverrideSerializer(serialize)

#define SE_REFLECTION_DELETE_SERIALIZER() \
OverrideSerializer(SE_REFLECTION_SUBSPACE::DeleteSerialize)

#define SE_REFLECTION_OVERRIDE_DISPOSER(dispose) \
OverrideDisposer(dispose)

#define SE_REFLECTION_DELETE_DISPOSER() \
OverrideDisposer(SE_REFLECTION_SUBSPACE::DeleteDisposal)

// Wrap up a reflection type definition.
#define SE_REFLECTION_END \
		Finalize();\
	return ptr;\
}


namespace SE_REFLECTION_SUBSPACE {

void* DeleteDeserializer(Engine::Core::DependencyInjection::ServiceProvider* services, Engine::Core::AssetManagement::ByteStream* source);
void DeleteIntializer(Engine::Core::DependencyInjection::ServiceProvider* services, void* asset);
void DeleteSerialize(Engine::Core::DependencyInjection::ServiceProvider* services, Engine::Core::AssetManagement::ByteStream* destination);
void DeleteDisposal(Engine::Core::DependencyInjection::ServiceProvider* services, void* data);

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

	inline ReflectionBuildingContext& OverrideDeserializer(void* (*deserialize)(Engine::Core::DependencyInjection::ServiceProvider* services, Engine::Core::AssetManagement::ByteStream* source))
	{
		Type.SerializerOverride.Deserialize = deserialize;
		return *this;
	}

	inline ReflectionBuildingContext& OverrideInitializer(void (*initialize)(Engine::Core::DependencyInjection::ServiceProvider* services, void* asset))
	{
		Type.SerializerOverride.Initialize = initialize;
		return *this;
	}

	inline ReflectionBuildingContext& OverrideSerializer(void (*serialize)(Engine::Core::DependencyInjection::ServiceProvider* services, Engine::Core::AssetManagement::ByteStream* destination))
	{
		Type.SerializerOverride.Serialize = serialize;
		return *this;
	}

	inline ReflectionBuildingContext& OverrideDisposer(void (*dispose)(Engine::Core::DependencyInjection::ServiceProvider* services, void* data))
	{
		Type.SerializerOverride.Disposal = dispose;
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
