#pragma once
#include "logger.h"

namespace SigourneyEngine
{
namespace Core
{
namespace Logging
{

class LoggerService
{
public:
	void Verbose(const char* channel, const char* message);
	void Information(const char* channel, const char* message);
	void Error(const char* channel, const char* message);
	void Warning(const char* channel, const char* message);
};

}
}
}
