#pragma once

#include "scriptable_property.h"

#include <string>
#include <vector>

namespace SigourneyEngine {
namespace Core {
namespace Reflection {

struct ScriptableType
{
	std::string Name;
	std::vector<ScriptableProperty> Properties;
};

}
}
}
