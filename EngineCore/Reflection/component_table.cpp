#include "component_table.h"

using namespace SigourneyEngine::Core::Reflection;

const ComponentDescriptor& ComponentTable::FindComponent(const std::string& name)
{
    auto result = m_ComponentMap.find(name);
    return result->second;
}
