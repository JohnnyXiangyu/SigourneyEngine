#pragma once

#include "pch.h"
#include "CppUnitTest.h"
#include <string>

namespace EngineCodeTests
{

struct TestLambda : Engine::FunctionalLayer::ILambda<bool, int>
{
    bool Run(int input) override
    {
        return input > 5;
    }
};

}
