#pragma once

#include "ErrorHandling/exceptions.h"
#include "Configuration/compile_time_flags.h"

#include <fstream>

namespace Engine {
namespace Core {
namespace AssetManagement {

/// <summary>
/// Implements the byte stream interface that'll be used by the rest of the engine for asset loading.
/// Hides away the implementation for asset storage.
/// Not using the libstd streaming infrastructure in favor of the c# style OOP approach.
/// </summary>
class ByteStream
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

	inline long long ReadAllAsString(std::string& dest)
	{
		char readBuffer[Configuration::STRING_LOAD_BUFFER_SIZE];

		long long totalReads = 0;
		long long newReads = 0;

		while ((newReads = Read(readBuffer, Core::Configuration::STRING_LOAD_BUFFER_SIZE - 1)) > 0)
		{
			readBuffer[newReads] = 0;
			dest.append(readBuffer);
			totalReads += newReads;
		}

		return totalReads;
	}

	bool FillBuffer(void* dest, size_t length);

	template <typename T>
	inline bool FillStruct(T* dest)
	{
		return FillBuffer(dest, sizeof(T));
	}

	template <typename T>
	inline bool FillArray(T* dest, size_t length)
	{
		return FillBuffer(dest, length * sizeof(T));
	}
};


class InFileStream : ByteStream
{
private:
	std::ifstream& m_Backend;

public:
	InFileStream(std::ifstream& backend);
	long long Read(char* buffer, long long count) override;
};


class OutFileStream : ByteStream
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
