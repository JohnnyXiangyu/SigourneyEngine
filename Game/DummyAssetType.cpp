#include "DummyAssetType.h"

#include <Reflection/type_info_registra.h>

using namespace SigourneyEngine::Game;

SE_REFLECTION_Begin(DummyAssetType)
.SE_REFLECTION_AddProperty(PartA)
.SE_REFLECTION_AddProperty(PartB)
.SE_REFLECTION_End
