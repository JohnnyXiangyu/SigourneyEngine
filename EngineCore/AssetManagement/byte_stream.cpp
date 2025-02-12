#include "byte_stream.h"

SigourneyEngine::Core::AssetManagement::ByteStream::ByteStream(std::ifstream& backend)
	: m_Backend(backend)
{
}

long long SigourneyEngine::Core::AssetManagement::ByteStream::Read(char* buffer, long long count)
{
	m_Backend.read(buffer, count);
	return m_Backend.gcount();
}
