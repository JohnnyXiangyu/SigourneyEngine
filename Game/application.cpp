#include "DummySinkModule.h"

#include <Hosting/engine_host.h>

using namespace SigourneyEngine::Core;
using namespace SigourneyEngine::Game;

int main()
{
	Hosting::EngineHost engineHost;
	engineHost.AddSinkModule<DummySinkModule>();
	return engineHost.Run();
}
