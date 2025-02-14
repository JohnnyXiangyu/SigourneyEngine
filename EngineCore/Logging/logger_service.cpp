#include "logger_service.h"

using namespace Engine::Core::Logging;

static LoggerService s_GlobalLogger;

LoggerService* Engine::Core::Logging::GetLogger()
{
    return &s_GlobalLogger;
}
