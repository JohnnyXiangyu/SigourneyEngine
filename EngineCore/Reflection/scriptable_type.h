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

struct ScriptableProperty
{
	std::string Name;
	DataType Type;
	unsigned int Offset;
};

struct ReferenceProperty
{
	std::string Name;
	const ScriptableType* TargetType;
	unsigned int Offset;
};

struct Serializer
{
	void* (*Deserialize)(DependencyInjection::ServiceProvider* services, AssetManagement::IByteStream* source) = nullptr;
	void (*Serialize)(DependencyInjection::ServiceProvider* services, AssetManagement::IByteStream* destination) = nullptr;
	void (*Disposal)(DependencyInjection::ServiceProvider* services, void* data) = nullptr;

	inline bool IsValid() const { return Deserialize && Serialize && Disposal; }
};

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
