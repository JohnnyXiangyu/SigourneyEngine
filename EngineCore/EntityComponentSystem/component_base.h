#pragma once

namespace Engine
{
namespace Core
{
namespace EntityComponentSystem
{

class ComponentRegistra
{
private:
	template <typename TComponent>
	friend struct Component;
	unsigned int Register();
};

template <typename TComponent>
struct Component
{
private:
	static unsigned int ComponentTypeID;

public:
	unsigned int EntityID;
	unsigned int ComponentID;
};

template <typename TComponent>
unsigned int Component<TComponent>::ComponentTypeID = ComponentRegistra::Register();

}
}
}
