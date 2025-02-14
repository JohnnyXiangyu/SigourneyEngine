#pragma once

#include <Memory/high_integrity_allocator.h>
#include <AssetManagement/byte_stream.h>
#include <Reflection/reference_type.h>

namespace Game {

struct DummyResType
{
	int Number;
};

struct DummyAssetType
{
	int PartA;
	Engine::Core::Reflection::Ref<DummyResType> PartB;
};

}
