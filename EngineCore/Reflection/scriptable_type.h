#pragma once

#include "data_type.h"

#include <string>
#include <vector>

namespace SigourneyEngine {
namespace Core {
namespace Reflection {

struct ScriptableProperty
{
	std::string Name;
	DataType Type;
	unsigned int Offset;
};

struct ScriptableType
{
	std::string Name;
	std::vector<ScriptableProperty> Properties;
};

}
}
}
