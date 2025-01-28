#include "logger_service.h"
#include <ctime>
#include <stdio.h>

using namespace SigourneyEngine::Core::Logging;

void LoggerService::Verbose(const char* channel, const char* message)
{
	time_t timeNow = time(0);
	tm now;
	localtime_s(&now, &timeNow);

	printf("[%d:%d:%d][VER][%s] %s\n", now.tm_hour, now.tm_min, now.tm_sec, channel, message);
}

void LoggerService::Information(const char* channel, const char* message)
{
	time_t timeNow = time(0);
	tm now;
	localtime_s(&now, &timeNow);

	printf("[%d:%d:%d][INF][%s] %s\n", now.tm_hour, now.tm_min, now.tm_sec, channel, message);
}

void LoggerService::Error(const char* channel, const char* message)
{
	time_t timeNow = time(0);
	tm now;
	localtime_s(&now, &timeNow);

	printf("[%d:%d:%d][ERR][%s] %s\n", now.tm_hour, now.tm_min, now.tm_sec, channel, message);
}

void LoggerService::Warning(const char* channel, const char* message)
{
	time_t timeNow = time(0);
	tm now;
	localtime_s(&now, &timeNow);

	printf("[%d:%d:%d][WRN][%s] %s\n", now.tm_hour, now.tm_min, now.tm_sec, channel, message);
}
