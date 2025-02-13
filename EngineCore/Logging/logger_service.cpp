#include "logger_service.h"

using namespace SigourneyEngine::Core::Logging;

static LoggerService s_GlobalLogger;

LoggerService* SigourneyEngine::Core::Logging::GetLogger()
{
    return &s_GlobalLogger;
}
