#pragma once

#include "CppUnitTest.h"

namespace EngineCodeTests
{
    class DummyPrintObserver : public SigourneyEngine::FunctionalLayer::StorageFreeEnumeration::IObserver<int>
    {
    public:
        void OnNext(int data) override;
    };

    class DummyProjectObserver : public SigourneyEngine::FunctionalLayer::StorageFreeEnumeration::ProjectionBase<char, int>
    {
    public:
        DummyProjectObserver(SigourneyEngine::FunctionalLayer::Memory::IAllocator* allocator) : SigourneyEngine::FunctionalLayer::StorageFreeEnumeration::ProjectionBase<char, int>(allocator) {}

        // Inherited via ProjectionBase
        int Transform(const char& sourceItem) override;
    };
}
