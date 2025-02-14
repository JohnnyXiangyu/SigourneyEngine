#pragma once
#include <ctime>
#include <iostream>

namespace Engine
{
namespace Core
{
namespace Logging
{

class LoggerService
{
private:
	template <typename... TArgs>
	void Log(const char* level, const char* channel, const char* message, TArgs... args)
	{
		time_t timeNow = time(0);
		tm now;
		localtime_s(&now, &timeNow);

		printf("[%d:%d:%d][%s][%s] ", now.tm_hour, now.tm_min, now.tm_sec, level, channel);
		printf(message, args...);
		printf("\n");
	}

public:
	template <typename... TArgs>
	void Verbose(const char* channel, const char* message, TArgs... args)
	{
		Log("VER", channel, message, args...);
	}

	template <typename... TArgs>
	void Information(const char* channel, const char* message, TArgs... args)
	{
		Log("INF", channel, message, args...);
	}
	
	template <typename... TArgs>
	void Error(const char* channel, const char* message, TArgs... args)
	{
		Log("ERR", channel, message, args...);
	}

	template <typename... TArgs>
	void Warning(const char* channel, const char* message, TArgs... args)
	{
		Log("WRN", channel, message, args...);
	}
};

LoggerService* GetLogger();

}
}
}
