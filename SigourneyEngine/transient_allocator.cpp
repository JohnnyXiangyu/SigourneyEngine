#include "transient_allocator.h"
#include <memory>

using namespace std;
using namespace SigourneyEngine::FunctionalLayer::Memory;

void* TransientAllocator::AllocateCore(size_t size)
{
    if (size == 0)
        return nullptr;

    // allocate and copy if there's not enough memory
    if (m_currentSize - m_currentUsage < size)
    {
        m_currentSize *= 2;
        char* newMemory = new char[m_currentSize];
        memcpy(newMemory, m_buffer, m_currentUsage);
        delete[] m_buffer;
        m_buffer = newMemory;
    }

    char* newPointer = m_buffer + m_currentUsage;
    m_currentUsage += size;
    return (void*)newPointer;
}

TransientAllocator::TransientAllocator(size_t initialCapacity)
{
    m_currentSize = initialCapacity;
    m_buffer = new char[m_currentSize];
}

TransientAllocator::~TransientAllocator()
{
    delete[] m_buffer;
}
