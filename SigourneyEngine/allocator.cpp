#include "allocator.h"
#include <memory>

using namespace std;


void* SigourneyEngine::FunctionalLayer::Memory::StaticAllocator::AllocateCore(size_t size)
{
    if (CurrentSize - CurrentUsage < size)
    {
        char* newBuffer = new char[CurrentSize * 2];
        memcpy(newBuffer, Buffer, CurrentUsage);
        delete[] Buffer;
        Buffer = newBuffer;
    }

    char* result = Buffer + CurrentUsage;
    CurrentUsage += size;
    return result;
}

SigourneyEngine::FunctionalLayer::Memory::StaticAllocator::StaticAllocator(size_t initialSize)
    : CurrentSize(initialSize), CurrentUsage(0)
{
    Buffer = new char[initialSize];
}

SigourneyEngine::FunctionalLayer::Memory::StaticAllocator::~StaticAllocator()
{
    delete[] Buffer;
}
