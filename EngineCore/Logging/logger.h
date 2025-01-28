#pragma once

namespace SigourneyEngine
{
namespace Core
{
namespace Logging
{

/// <summary>
/// Very basic logger interface used in engine core code; not strcutured.
/// </summary>
class Logger
{
public:
	static void Verbose(const char* message);
	static void Information(const char* message);
	static void Error(const char* message);
	static void Warning(const char* message);
};

}
}
}
