#include "byte_stream.h"

using namespace Engine::Core::AssetManagement;

Engine::Core::AssetManagement::InFileStream::InFileStream(std::ifstream& backend)
	: m_Backend(backend)
{
}

long long InFileStream::Read(char* buffer, long long count)
{
	m_Backend.read(buffer, count);
	return m_Backend.gcount();
}

OutFileStream::OutFileStream(std::ofstream& backend)
	: m_Backend(backend)
{
}

void OutFileStream::Write(char* buffer, long long count)
{
	m_Backend.write(buffer, count);
}

bool ByteStream::FillBuffer(void* dest, size_t length)
{
	char* destBuffer = (char*)dest;

	size_t totalReads = 0;
	size_t newReads = 0;

	while (totalReads < length && (newReads = Read(destBuffer + totalReads, length)) > 0)
	{
		totalReads += newReads;
	}

	return totalReads == length;
}
