#include "pch.h"
#include "CppUnitTest.h"
#include "dummy_classes.h"

using namespace std;
using namespace EngineCodeTests;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace SigourneyEngine::FunctionalLayer;
using namespace SigourneyEngine::Utils;

namespace EngineCodeTests
{

TEST_CLASS(FunctionalLayerTests)
{
public:
	TEST_METHOD(BasicEnumerableTest)
	{
		Memory::HighIntegrityAllocator allocator(1);
		RuntimeBase runtime(&allocator);
		Enumeration::ArrayLiteral<double, 10>* arr = runtime.MakeArray<double, 10>();

		Logger::WriteMessage(std::to_string(arr->GetCount()).c_str());
		Logger::WriteMessage("\n");

		Assert::IsTrue(arr->GetCount() == 10);

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

		auto enumerator2 = arr->InitializeEnumerator(runtime.GetAllocator());
		while (true)
		{
			Logger::WriteMessage(std::to_string(arr->Dereference(enumerator2)).c_str());
			Logger::WriteMessage("\n");

			if (!arr->IncrementEnumerator(enumerator2))
				break;
		}

		arr->FinalizeEnumerator(enumerator, runtime.GetAllocator());
		arr->FinalizeEnumerator(enumerator2, runtime.GetAllocator());
	}

	TEST_METHOD(SelectEnumerableTest)
	{
		Memory::HighIntegrityAllocator allocator(1);
		RuntimeBase runtime(&allocator);
		Enumeration::ArrayLiteral<int, 10>* arr = runtime.MakeArray<int, 10>();

		Assert::IsTrue(arr->GetCount() == 10);

		for (int i = 0; i < 10; i++)
		{
			arr->SetItem(i, i * 3);
		}

		Enumeration::SelectEnumerable<int, bool>* selector = runtime.Select<int, bool>(arr, TestLambda{});
		Enumeration::IEnumerator* enumerator = selector->InitializeEnumerator(&allocator);
		while (true)
		{
			int nextValue = selector->Dereference(enumerator);

			Logger::WriteMessage(std::to_string(nextValue).c_str());
			Logger::WriteMessage("\n");

			if (!selector->IncrementEnumerator(enumerator))
				break;
		}

		selector->FinalizeEnumerator(enumerator, &allocator);
	}

	TEST_METHOD(FilterEnumerableTest)
	{
		Memory::HighIntegrityAllocator allocator(1);
		RuntimeBase runtime(&allocator);
		Enumeration::ArrayLiteral<int, 10>* arr = runtime.MakeArray<int, 10>();

		Assert::IsTrue(arr->GetCount() == 10);

		for (int i = 0; i < 10; i++)
		{
			arr->SetItem(i, i);
		}

		Enumeration::FilterEnumerable<int>* selector = runtime.Where<int>(arr, TestLambda{});
		Enumeration::IEnumerator* enumerator = selector->InitializeEnumerator(&allocator);
		while (true)
		{
			int nextValue = selector->Dereference(enumerator);

			Logger::WriteMessage(std::to_string(nextValue).c_str());
			Logger::WriteMessage("\n");

			if (!selector->IncrementEnumerator(enumerator))
				break;
		}

		selector->FinalizeEnumerator(enumerator, &allocator);
	}
};

}
