#include "FunctionalLayer/runtime.h"
#include "EntityComponentSystem/component_controller.h"
#include <stdio.h>

using namespace SigourneyEngine::Memory;
using namespace SigourneyEngine::FunctionalLayer;
using namespace SigourneyEngine::EntityComponentSystem;

int main()
{
	printf("main begin\n");

	// TODO: run the update once on a generated routine
	HighIntegrityAllocator globalAllocator(5);
	ComponentController controller(&globalAllocator);

	HighIntegrityAllocator functionalAllocator(4);
	RuntimeBase runtime(&functionalAllocator, &controller);

	printf("main end\n");
	return 0;
}
