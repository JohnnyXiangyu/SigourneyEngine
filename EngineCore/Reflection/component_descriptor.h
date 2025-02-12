#pragma once
#include <string>
#include <vector>

#include "primitive_types.h"

namespace SigourneyEngine
{
namespace Core
{
namespace Reflection
{

/// <summary>
/// A component property is only allowed to be primitive type.
/// </summary>
struct PropertyDescriptor
{
	std::string Name;
	PrimitiveType Type;
};

/// <summary>
/// Component name and a vector of properties.
/// </summary>
struct ComponentDescriptor
{
	std::string Name;
	std::vector<PropertyDescriptor> Properties;
};

}
}
}
