#pragma once

#include "scriptable_type.h"
#include <string>

namespace Engine {
namespace Core {
namespace Reflection {

template <typename T>
const ScriptableType* GetType();

const ScriptableType* GetType(std::string name);

}
}
}
