#pragma once

#include "data_type.h"

#include <string>

namespace SigourneyEngine {
namespace Core {
namespace Reflection {

struct ScriptableProperty
{
	std::string Name;
	DataType Type;
	unsigned int Offset;
};

}
}
}