#pragma once

namespace Engine
{
namespace Core
{
namespace Utils
{

template <typename THeader>
inline THeader* GetHeader(void* original)
{
    return (THeader*)(((char*)original) - sizeof(THeader));
}

template <typename THeader>
inline void* SkipHeader(THeader* node)
{
    return (void*)(((char*)node) + sizeof(THeader));
}

}
}
}
