#include "DummySinkModule.h"

#include <Hosting/engine_host.h>

using namespace Engine::Core;
using namespace Game;

int main()
{
	Hosting::EngineHost engineHost;
	engineHost.AddSinkModule<DummySinkModule>();
	return engineHost.Run();
}
