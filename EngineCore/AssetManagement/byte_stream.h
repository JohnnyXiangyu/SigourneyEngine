#pragma once

#include "ErrorHandling/exceptions.h"

#include <fstream>

namespace Engine {
namespace Core {
namespace AssetManagement {

/// <summary>
/// Implements the byte stream interface that'll be used by the rest of the engine for asset loading.
/// Hides away the implementation for asset storage.
/// Not using the libstd streaming infrastructure in favor of the c# style OOP approach.
/// </summary>
class IByteStream
{
protected:
	bool m_CanRead = false;
	bool m_CanWrite = false;

public:
	inline bool CanRead() const { return m_CanRead; }
	inline bool CanWrite() const { return m_CanWrite; }

	virtual long long Read(char* buffer, long long count)
	{
		SE_THROW_NOT_IMPLEMENTED;
	}

	virtual void Write(char* buffer, long long count)
	{
		SE_THROW_NOT_IMPLEMENTED;
	}
};


class InFileStream : IByteStream
{
private:
	std::ifstream& m_Backend;

public:
	InFileStream(std::ifstream& backend);
	long long Read(char* buffer, long long count) override;
};

class OutFileStream : IByteStream
{
private:
	std::ofstream& m_Backend;

public:
	OutFileStream(std::ofstream& backend);
	void Write(char* buffer, long long count) override;
};


}
}
}
