#pragma once

#include "homogeneous_storage.h"

//#define TEMPLATE_DEVELOPEMENT_SITUATION 10

namespace SigourneyEngine {
namespace EntityComponentSystem {

// please don't back fire
#ifdef TEMPLATE_DEVELOPEMENT_SITUATION 

struct TComponent
{

};

#else

template <typename TComponent>

#endif

/// <summary>
/// Component stores implement IEnumerable interface and uses its own memory allocation system.
/// </summary>
class ComponentStore
{

};

}
}
