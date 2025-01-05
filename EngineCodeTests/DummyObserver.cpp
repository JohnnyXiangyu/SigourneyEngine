#include "pch.h"
#include "DummyObserver.h"
#include "CppUnitTest.h"
#include<string>

using namespace std;

int EngineCodeTests::DummyProjectObserver::Transform(const char& sourceItem)
{
    return (int)sourceItem;
}

void EngineCodeTests::DummyPrintObserver::OnNext(int data)
{
    Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(to_string(data).c_str());
    Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage("\n");
}
