#pragma once

#include <string>
#include <vector>

namespace Engine {
namespace Core {
namespace AssetManagement {

struct CompilableElement
{
	std::string Name;
	
};

struct DataCompileInstruction
{
	std::vector<CompilableElement> Elements;
};


template <typename T>
struct ElementDescriptor
{
	std::string Name;

	T Load();
};

template <typename... TArgs>
static void RegisterScriptObject()
{

}



}
}
}
