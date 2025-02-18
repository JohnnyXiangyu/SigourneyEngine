#include "DummySinkModule.h"

#include <Hosting/engine_host.h>
#include <renderer_module.h>

using namespace Engine::Core;
using namespace Game;

int main()
{
	Hosting::EngineHost engineHost;
	engineHost.AddSinkModule<Extension::RendererModule::RendererModule>();
	//engineHost.AddSinkModule<DummySinkModule>();
	return engineHost.Run();
}
