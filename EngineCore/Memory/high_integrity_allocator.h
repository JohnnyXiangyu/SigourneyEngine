#pragma once

#include <utility>
#include <unordered_map>

namespace Engine {
namespace Core {
namespace Memory {

class HomogeneousStorage;

class HighIntegrityAllocator
{
private:
    unsigned int m_InitialBufferItemCount;
    std::unordered_map<size_t, HomogeneousStorage*> m_BufferTable;

    void* AllocateCore(size_t size);


public:
    HighIntegrityAllocator(unsigned int initialCount);
    ~HighIntegrityAllocator();
    
    template <typename T>
    T* New(const T&& copy)
    {
        void* newPayload = AllocateCore(sizeof(T));
        return new (newPayload) T(copy);
    }
    
    template <typename T>
    T* New(const T& copy)
    {
        void* newPayload = AllocateCore(sizeof(T));
        return new (newPayload) T(copy);
    }

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
        void* newPayload = AllocateCore(sizeof(T));
        return new (newPayload) T(std::forward<TArgs>(args)...);
    }



    /// <summary>
    /// Allocate an empty space in memory with a set size.
    /// </summary>
    /// <param name="size"></param>
    /// <returns></returns>
    void* Malloc(size_t size)
    {
        return AllocateCore(size);
    }

    /// <summary>
    /// Free the buffer previously allocated by the allocator.
    /// </summary>
    /// <param name="pointer">the ticket of the returned object</param>
    void Free(void* pointer);
};

}
}
}
