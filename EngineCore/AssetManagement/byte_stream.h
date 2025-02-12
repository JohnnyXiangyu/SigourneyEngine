#pragma once

#include <fstream>

namespace SigourneyEngine {
namespace Core {
namespace AssetManagement {

/// <summary>
/// Implements the byte stream interface that'll be used by the rest of the engine for asset loading.
/// Hides away the implementation for asset storage.
/// </summary>
class ByteStream
{
private:
	// temp solution: file stream backend
	std::ifstream& m_Backend;

public:
	ByteStream(std::ifstream& backend);
	long long Read(char* buffer, long long count);
};

}
}
}
