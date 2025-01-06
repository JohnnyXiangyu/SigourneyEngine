#include "pch.h"
#include "CppUnitTest.h"
#include "dummy_classes.h"

using namespace EngineCodeTests;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace SigourneyEngine::FunctionalLayer;

namespace EngineCodeTests
{
TEST_CLASS(FunctionalLayerTests)
{
public:
	TEST_METHOD(BasicObserverTest)
	{
		DummySource source;
		DummyProject plus1(1);
		DummyProject minus1(-1);
		DummyProject plus10(10);
		DummyProject minus10(-10);
		DummySink sink;

		Memory::StaticAllocator allocator(128);

		Enumeration::Subscribe(&source.Observable, &plus1.Observer, &allocator);
		Enumeration::Subscribe(&plus1.Observable, &sink.Observer, &allocator);
		
		Enumeration::Subscribe(&source.Observable, &minus1.Observer, &allocator);
		Enumeration::Subscribe(&minus1.Observable, &sink.Observer, &allocator);
		
		Enumeration::Subscribe(&plus1.Observable, &plus10.Observer, &allocator);
		Enumeration::Subscribe(&plus10.Observable, &sink.Observer, &allocator);
		
		Enumeration::Subscribe(&minus1.Observable, &minus10.Observer, &allocator);
		Enumeration::Subscribe(&minus10.Observable, &sink.Observer, &allocator);

		source.PipeThrough();
	}
};
}
