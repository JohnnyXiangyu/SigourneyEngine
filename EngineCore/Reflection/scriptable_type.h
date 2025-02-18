#pragma once

#include "data_type.h"
#include "AssetManagement/byte_stream.h"

#include <string>
#include <vector>

namespace Engine {
namespace Core {

namespace DependencyInjection {

class ServiceProvider;

}

namespace Reflection {

struct ScriptableType;


/// <summary>
/// A property with a simple data type (defined by the engine).
/// Data types are always directly serialized and deserialized locally without consulting other data structures.
/// </summary>
struct ScriptableProperty
{
	std::string Name;
	DataType Type;
	unsigned int Offset;
};


/// <summary>
/// A property that refrences another reflected type. 
/// </summary>
struct ReferenceProperty
{
	std::string Name;
	const ScriptableType* TargetType;
	unsigned int Offset;
};


/// <summary>
/// A serializer is a set of functions that allow the engine to dynamically deserialize and serialize the data *represented by* the reflected type.
/// Most commonly it'll be used for asset types such as shaders or meshes.
/// </summary>
struct Serializer
{
	void* (*Deserialize)(DependencyInjection::ServiceProvider* services, AssetManagement::ByteStream* source) = nullptr;
	void (*Initialize)(DependencyInjection::ServiceProvider* services, void* asset) = nullptr;
	void (*Serialize)(DependencyInjection::ServiceProvider* services, AssetManagement::ByteStream* destination) = nullptr;
	void (*Disposal)(DependencyInjection::ServiceProvider* services, void* data) = nullptr;
};


/// <summary>
/// Scriptable types are reflected types that allow the engine to dynamically create and destruct; they are meant for simple struct types such as components and asset types.
/// </summary>
struct ScriptableType
{
	std::string Name;
	size_t Size;
	void* (*GetCDO)(void* buffer);
	std::vector<ScriptableProperty> Properties;
	std::vector<ReferenceProperty> References;
	Serializer SerializerOverride;
};

}
}
}
