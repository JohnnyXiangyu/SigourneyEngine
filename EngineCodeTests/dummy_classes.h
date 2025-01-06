#pragma once

#include "pch.h"
#include "CppUnitTest.h"
#include <string>

namespace EngineCodeTests
{

struct DummySource
{
    SigourneyEngine::FunctionalLayer::Enumeration::ObservableHeader<int> Observable;
    
    void PipeThrough()
    {
        for (int i = 0; i < 5; i++)
        {
            Observable.Notify(i);
        }
    }
};

struct DummyProject
{
    SigourneyEngine::FunctionalLayer::Enumeration::ObserverHeader<int> Observer;
    SigourneyEngine::FunctionalLayer::Enumeration::ObservableHeader<int> Observable;
    int Added = 0;

    static void OnNextRoutine(void* context, int item)
    {
        DummyProject* self = (DummyProject*)context;
        int processedItem = item + self->Added;
        self->Observable.Notify(processedItem);
    }

    DummyProject(int add) : Added(add), Observer(OnNextRoutine) {}
};

struct DummySink
{
    SigourneyEngine::FunctionalLayer::Enumeration::ObserverHeader<int> Observer;

    static void OnNextRoutine(void* context, int item)
    {
        Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(std::to_string(item).c_str());
        Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage("\n");
    }

    DummySink() : Observer(OnNextRoutine) {}
};

}
