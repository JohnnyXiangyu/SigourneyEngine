#pragma once

#include "../Memory/high_integrity_allocator.h"
#include "../Enumeration/enumerable.h"
#include "../Utils/shorthand_functions.h"

//#define TEMPLATE_DEVELOPEMENT_SITUATION 10

namespace SigourneyEngine {
namespace EntityComponentSystem {

/// <summary>
/// Component stores implement IEnumerable interface and uses its own memory allocation system.
/// </summary>
template <typename TComponent>
class ComponentStore final : public Enumeration::IEnumerable<TComponent>
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

    struct Enumerator : SigourneyEngine::Enumeration::IEnumerator
    {
        Payload* Item = nullptr;
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

    // begin: IEnumerable interface
    Enumeration::IEnumerator* InitializeEnumerator(Memory::HighIntegrityAllocator* allocator) override
    {
        if (m_head == nullptr)
            return nullptr;

        Enumerator* newEnumerator = allocator->New<Enumerator>();
        newEnumerator->Item = m_head;
        return newEnumerator;
    }
    
    void FinalizeEnumerator(Enumeration::IEnumerator* enumerator, Memory::HighIntegrityAllocator* allocator) override
    {
        allocator->Free(enumerator);
    }

    TComponent Dereference(SigourneyEngine::Enumeration::IEnumerator* enumerator) override
    {
        Enumerator* castEnumerator = (Enumerator*)enumerator;
        return castEnumerator->Item->Component;
    }
    
    bool IncrementEnumerator(SigourneyEngine::Enumeration::IEnumerator* enumerator) override
    {
        Enumerator* castEnumerator = (Enumerator*)enumerator;
        if (castEnumerator->Item->Header.Next == nullptr)
            return false;

        castEnumerator->Item = (Payload*)castEnumerator->Item->Header.Next;
        return true;
    }
};

}
}
