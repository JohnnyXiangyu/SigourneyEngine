#include "pch.h"
#include "CppUnitTest.h"
#include "dummy_classes.h"

using namespace std;
using namespace EngineCodeTests;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace SigourneyEngine::FunctionalLayer;
using namespace SigourneyEngine::Utils;
using namespace SigourneyEngine::Memory;
using namespace SigourneyEngine::Enumeration;

namespace EngineCodeTests
{

TEST_CLASS(FunctionalLayerTests)
{
public:
	TEST_METHOD(BasicEnumerableTest)
	{
		HighIntegrityAllocator allocator(1);
		RuntimeBase runtime(&allocator, nullptr);
		ArrayLiteral<double, 10>* arr = runtime.MakeArray<double, 10>();

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
		HighIntegrityAllocator allocator(1);
		RuntimeBase runtime(&allocator, nullptr);
		ArrayLiteral<int, 10>* arr = runtime.MakeArray<int, 10>();

		Assert::IsTrue(arr->GetCount() == 10);

		for (int i = 0; i < 10; i++)
		{
			arr->SetItem(i, i * 3);
		}

		TestLambda testSelectLambda{};

		SelectEnumerable<int, bool>* selector = runtime.Select<int, bool>(arr, &testSelectLambda);
		IEnumerator* enumerator = selector->InitializeEnumerator(&allocator);
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
		HighIntegrityAllocator allocator(1);
		RuntimeBase runtime(&allocator, nullptr);
		ArrayLiteral<int, 10>* arr = runtime.MakeArray<int, 10>();

		Assert::IsTrue(arr->GetCount() == 10);

		for (int i = 0; i < 10; i++)
		{
			arr->SetItem(i, i);
		}

		TestLambda testLambda{};

		FilterEnumerable<int>* selector = runtime.Where<int>(arr, &testLambda);
		IEnumerator* enumerator = selector->InitializeEnumerator(&allocator);
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

	template <typename TRet, typename ...TArgs>
	struct LambdaConstructor
	{
		template <typename TLambda>
		static ILambda<TRet, TArgs...>* Construct(HighIntegrityAllocator* allocator, const TLambda&& lambda)
		{
			return (ILambda<TRet, TArgs ...>*) allocator->New<LambdaClosure<TLambda, TRet, TArgs...>>(lambda);
		}
	};

	TEST_METHOD(ClosureForward)
	{
		int a = 10;
		int b = 20;
		int c = 30;
		double d = 1.5;

		HighIntegrityAllocator allocator(1);
		auto* lambda = LambdaConstructor<double, double>::Construct(&allocator, [a, b, c, d](double e)
			{
				double result = a * b * c * d;
				return result + e;
			});
		double value = lambda->Run(10);
		Logger::WriteMessage(std::to_string(value).c_str());
	}
};

}
