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

struct BaseAcceptor
{
    bool Accept(const double& element)
    {
        Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(std::to_string(element).c_str());
        Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage("\n");
        return true;
    }
};

struct BaseTransform
{
    double Transform(int element) const
    {
        return element * 3.1415926;
    }
};

struct BasePredicate
{
    bool Allow(double element) const
    {
        return element > 6.18;
    }
};

struct HeadAcceptor
{
    int Counter = 0;

    bool Accept(const double& element)
    {
        Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(std::to_string(element).c_str());
        Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage("\n");
        Counter++;
        return Counter < 2;
    }
};

}
