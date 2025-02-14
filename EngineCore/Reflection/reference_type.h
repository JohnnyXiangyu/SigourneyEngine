#pragma once

#include "scriptable_type.h"
#include "reflection.h"

namespace Engine {
namespace Core {
namespace Reflection {

/// <summary>
/// Wrap a member pointer inside a template struct to give us some compile time checks.
/// </summary>
template <typename T>
struct Ref
{
	T* Data = nullptr;
	
	const ScriptableType* GetType()
	{
		return Reflection::GetType<T>();
	}
};

}
}
}
