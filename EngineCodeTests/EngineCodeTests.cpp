#include "pch.h"
#include "CppUnitTest.h"
#include "dummy_classes.h"

using namespace std;
using namespace EngineCodeTests;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace SigourneyEngine::FunctionalLayer;

namespace EngineCodeTests
{

TEST_CLASS(FunctionalLayerTests)
{
public:
	TEST_METHOD(BasicEnumerableTest)
	{
		Memory::StaticAllocator allocator(128);
		RuntimeBase runtime(&allocator);
		Enumeration::ArrayLiteral<double, 10>* arr = runtime.MakeArray<double, 10>();

		Logger::WriteMessage(std::to_string(arr->Count()).c_str());
		Logger::WriteMessage("\n");

		Assert::IsTrue(arr->Count() == 10);

		for (int i = 0; i < 10; i++)
		{
			arr->SetItem(i, i * 3.14);
		}

		auto enumerator = arr->InitializeEnumerator(runtime.GetAllocator());
		while (true)
		{
			Logger::WriteMessage(std::to_string(arr->Dereference(enumerator)).c_str());
			Logger::WriteMessage("\n");

			if (!arr->IncrementEnumerator(enumerator))
				break;
		}

		arr->FinalizeEnumerator(enumerator, runtime.GetAllocator());
	}
};

}
