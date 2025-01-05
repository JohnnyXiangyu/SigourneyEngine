#include "pch.h"
#include "CppUnitTest.h"
#include "DummyObserver.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace SigourneyEngine::FunctionalLayer::StorageFreeEnumeration;
using namespace SigourneyEngine::FunctionalLayer::Memory;

namespace EngineCodeTests
{
	TEST_CLASS(FunctionalLayerTests)
	{
	public:
		TEST_METHOD(BasicObserverTest)
		{
			TransientAllocator allocator(128);

			DummyPrintObserver observerOne;
			DummyPrintObserver observerTwo;

			DummyProjectObserver projector((IAllocator*)&allocator);

			projector.Subscribe(& observerOne);
			projector.Subscribe(& observerTwo);

			projector.OnNext('a');
			projector.OnNext('b');
			projector.OnNext('c');
			projector.OnNext('d');
			projector.OnNext('e');
		}
	};
}
