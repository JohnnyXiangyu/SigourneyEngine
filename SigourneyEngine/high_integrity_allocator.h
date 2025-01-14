#pragma once

#include "allocation_table.h"

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Memory {

class BufferChainSegment;

/// <summary>
/// Implements IAllocator interface; uses an allocation design that eliminates memory fragmentation;
/// may be refered to as H.I.A or HIA in other parts of engine code
/// </summary>
class HighIntegrityAllocator
{
private:
    // the class descriptor is translated to size descriptor so we have fewer table entry numbers hanging around in shared memory
    template <unsigned int T>
    struct SizeDescriptor
    {
        static unsigned int TableEntry;
    };

    // use the template instantiation in C++ to help us automatically gather information about all the client tyeps using H.I.A
    template <typename T>
    struct ClassDescriptor
    {
        static unsigned int GetTableEntry() { return SizeDescriptor<sizeof(T)>::TableEntry; }
    };

    unsigned int m_initialBufferItemCount;
    BufferChainSegment** m_bufferTable;

    void* AllocateCore(unsigned int tableEntry, size_t payloadSize);

public:
    HighIntegrityAllocator(unsigned int initialCount);

    ~HighIntegrityAllocator();

    /// <summary>
    /// Allocates a buffer to hold one instance of T, then run a placement-new on that buffer;
    /// Please use the typedef to label which type this ticket points to.
    /// </summary>
    /// <typeparam name="T">target type</typeparam>
    /// <typeparam name="...TArgs">variable length argument list forwarded to new() constructor</typeparam>
    /// <param name="...args">variable length argument list forwarded to new() constructor</param>
    /// <returns>Returns a ticket that can be translated to a member address of that newly allocated T object.</returns>
    template <typename T, typename ...TArgs>
    T* New(TArgs... args)
    {
        unsigned int tableEntry = ClassDescriptor<T>::GetTableEntry();
        void* newPayload = AllocateCore(tableEntry, sizeof(T));
        return new (newPayload) T(args...);
    }

    /// <summary>
    /// Free the buffer previously allocated by the allocator.
    /// </summary>
    /// <param name="pointer">the ticket of the returned object</param>
    void Free(void* pointer);
};

template <unsigned int T>
unsigned int HighIntegrityAllocator::SizeDescriptor<T>::TableEntry = AllocationTable::GetSingleton()->RegisterType(T);

}
}
}
