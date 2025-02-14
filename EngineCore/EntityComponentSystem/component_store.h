#pragma once

#include "Memory/high_integrity_allocator.h"
#include "Utils/shorthand_functions.h"

namespace Engine
{
namespace Core
{
namespace EntityComponentSystem
{

/// <summary>
/// Component stores implement IEnumerable interface and uses its own memory allocation system.
/// </summary>
template <typename TComponent>
class ComponentStore
{
private:
    Memory::HighIntegrityAllocator* m_Allocator;

    struct Header
    {
        Header* Prev = nullptr;
        Header* Next = nullptr;
    };

    struct Payload
    {
        Header Header;
        TComponent Component;

        Payload(const TComponent& initialValue) : Component(initialValue) {}
        Payload(const TComponent&& initialValue) : Component(initialValue) {}
    };

    Payload* m_head = nullptr;
    Payload* m_tail = nullptr;

public:
    ComponentStore(Memory::HighIntegrityAllocator* injectAllocator) : m_Allocator(injectAllocator) {}

    TComponent* CreateComponent(const TComponent& initialValue)
    {
        Payload* newComponent = m_Allocator->New<Payload>(initialValue);

        newComponent->Header.Prev = &m_tail->Header;
        m_tail->Header.Next = &newComponent->Header;
        m_tail = newComponent;

        return &newComponent->Component;
    }

    TComponent* CreateComponent(const TComponent&& initialValue)
    {
        Payload* newComponent = m_Allocator->New<Payload>(initialValue);

        newComponent->Header.Prev = &m_tail->Header;
        m_tail->Header.Next = &newComponent->Header;
        m_tail = newComponent;

        return &newComponent->Component;
    }

    void RemoveComponent(TComponent* target)
    {
        Header* header = Utils::GetHeader<Header>(target);

        if (header->Prev != nullptr)
        {
            header->Prev->Next = header->Next;
        }
        if (header->Next != nullptr)
        {
            header->Next->Prev = header->Prev;
        }

        m_Allocator->Free(header);
    }
};

}
}
}
