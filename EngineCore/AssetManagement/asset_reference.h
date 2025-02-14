#pragma once

namespace Engine {
namespace Core {
namespace AssetManagement {


/// <summary>
/// For internal use only, keep this in sync with template AssetReference
/// </summary>
struct UntypedAssetReference
{
	void* Data;
};


/// <summary>
/// A reference to an asset type.
/// It's mostly just a type-hinted synonym for a pointer of tha actual data asset.
/// Since assets are managed by the asset manager, they don't need to be freed when the refernce is freed;
/// And because asset references are calculated in scene, we don't have to worry about a referenced asset being freed from memory when the reference is still valid.
/// </summary>
/// <typeparam name="TAsset">the type this asset should deserialize to</typeparam>
template <typename TAsset>
struct AssetReference
{
	TAsset* Data;
};

}
}
}
