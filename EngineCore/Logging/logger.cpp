#include "logger.h"
#include <ctime>
#include <stdio.h>

using namespace SigourneyEngine::Core::Logging;

void Logger::Verbose(const char* message)
{
	time_t timeNow = time(0);
	tm now;
	localtime_s(&now, &timeNow);

	printf("[%d:%d:%d][VER] %s\n", now.tm_hour, now.tm_min, now.tm_sec, message);
}

void Logger::Information(const char* message)
{
	time_t timeNow = time(0);
	tm now;
	localtime_s(&now, &timeNow);

	printf("[%d:%d:%d][INF] %s\n", now.tm_hour, now.tm_min, now.tm_sec, message);
}

void Logger::Error(const char* message)
{
	time_t timeNow = time(0);
	tm now;
	localtime_s(&now, &timeNow);

	printf("[%d:%d:%d][ERR] %s\n", now.tm_hour, now.tm_min, now.tm_sec, message);
}

void Logger::Warning(const char* message)
{
	time_t timeNow = time(0);
	tm now;
	localtime_s(&now, &timeNow);

	printf("[%d:%d:%d][WRN] %s\n", now.tm_hour, now.tm_min, now.tm_sec, message);
}
