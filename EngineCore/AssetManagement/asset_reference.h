#pragma once


namespace SigourneyEngine {
namespace Core {
namespace AssetManagement {

/// <summary>
/// A reference to a *loaded* asset type.
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
