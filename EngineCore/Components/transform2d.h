#pragma once
#include "../Math/vector_2d.h"
#include "../EntityComponentSystem/component_base.h"

namespace SigourneyEngine
{
namespace Core
{
namespace Shared
{

struct Transform2D : Core::EntityComponentSystem::Component<Transform2D>
{
	Core::Math::Matrix22 Translation;
	Core::Math::Matrix22 Rotation;
	Core::Math::Matrix22 Scale;
	unsigned int ParentComponentID;
};

}
}
}
