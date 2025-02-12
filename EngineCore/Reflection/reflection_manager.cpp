#include "reflection_manager.h"

using namespace SigourneyEngine::Core::Reflection;

const char s_ChannelName[] = "ReflectionManager";

const ScriptableType* SigourneyEngine::Core::Reflection::ReflectionManager::GetType(std::string name)
{
	auto found = m_ReflectionTypes.find(name);
	if (found == m_ReflectionTypes.end())
		return nullptr;

	return &found->second;
}
