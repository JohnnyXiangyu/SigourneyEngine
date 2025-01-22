#pragma once

namespace SigourneyEngine {
namespace Memory {

class HomogeneousStorage;

/// <summary>
/// Implements IAllocator interface; uses an allocation design that eliminates memory fragmentation;
/// may be refered to as H.I.A or HIA in other parts of engine code
/// </summary>
class HighIntegrityAllocator
{
private:
    template <unsigned int T>
    struct SizeEquivalentClass
    {
        static unsigned int TableEntry;
    };

    unsigned int m_initialBufferItemCount;
    SigourneyEngine::Memory::HomogeneousStorage* m_bufferTable;

    void* AllocateCore(unsigned int tableEntry);

    static unsigned int GetTableEntryCore(unsigned int size);

    template <typename T>
    unsigned int GetTableEntry() { return SizeEquivalentClass<sizeof(T)>::TableEntry; }

public:
    HighIntegrityAllocator(unsigned int initialCount);

    ~HighIntegrityAllocator();

    /// <summary>
    /// Finds a new buffer for at least size T and initialize it.
    /// </summary>
    /// <typeparam name="T">target type</typeparam>
    /// <typeparam name="...TArgs">variable length argument list forwarded to new() constructor</typeparam>
    /// <param name="...args">variable length argument list forwarded to new() constructor</param>
    /// <returns>Pointer of a buffer with available size at least sizeof(T).</returns>
    template <typename T, typename ...TArgs>
    T* New(TArgs... args)
    {
        unsigned int tableEntry = GetTableEntry<T>();
        void* newPayload = AllocateCore(tableEntry);
        return new (newPayload) T(args...);
    }

    /// <summary>
    /// Free the buffer previously allocated by the allocator.
    /// </summary>
    /// <param name="pointer">the ticket of the returned object</param>
    void Free(void* pointer);
};

template <unsigned int T>
unsigned int HighIntegrityAllocator::SizeEquivalentClass<T>::TableEntry = HighIntegrityAllocator::GetTableEntryCore(T);

}
}
