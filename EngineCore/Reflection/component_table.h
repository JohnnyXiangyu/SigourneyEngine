#pragma once

#include "component_descriptor.h"

#include <unordered_map>

namespace SigourneyEngine
{
namespace Core
{
namespace Reflection
{

/// <summary>
/// ComponentTable handles lookup for components by their names;
/// </summary>
class ComponentTable
{
private:
	std::unordered_map<std::string, ComponentDescriptor> m_ComponentMap;

public:
	const ComponentDescriptor& FindComponent(const std::string& name);
};

}
}
}
