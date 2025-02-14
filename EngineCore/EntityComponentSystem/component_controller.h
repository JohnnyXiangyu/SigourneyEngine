#pragma once

#include "component_store.h"
#include "Memory/high_integrity_allocator.h"

namespace Engine
{
namespace Core
{
namespace EntityComponentSystem
{

class ComponentController
{
    // typing system
private:
    template <typename TComponent>
    friend struct ComponentDescriptor;
    static unsigned int RegisterType();

    // component description
private:
    static unsigned int s_ComponentCount;

    template <typename TComponent>
    struct ComponentDescriptor
    {
        static unsigned int ID;
    };

    // center storage
private:
    Memory::HighIntegrityAllocator* m_Allocator;
    void** m_ComponentStores = new void* [s_ComponentCount];

public:
    ComponentController(Memory::HighIntegrityAllocator* injectAllocator) : m_Allocator(injectAllocator) {}

    template <typename TComponent>
    ComponentStore<TComponent>* GetStore()
    {
        unsigned int storeId = ComponentDescriptor<TComponent>::ID;

        if (m_ComponentStores[storeId] == nullptr)
        {
            m_ComponentStores[storeId] = m_Allocator->New<ComponentStore<TComponent>>(m_Allocator);
        }

        return (ComponentStore<TComponent>*) m_ComponentStores[storeId];
    }

    ~ComponentController()
    {
        // free all component stores
        for (unsigned int i = 0; i < s_ComponentCount; i++)
        {
            if (m_ComponentStores[i] != nullptr)
            {
                m_Allocator->Free(m_ComponentStores[i]);
            }
        }
    }
};

template <typename TComponent>
unsigned int ComponentController::ComponentDescriptor<TComponent>::ID = ComponentController::RegisterType();

}
}
}
