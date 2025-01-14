#include "allocation_table.h"

// singleton instance
static SigourneyEngine::FunctionalLayer::Memory::AllocationTable MetaTable;

void SigourneyEngine::FunctionalLayer::Memory::AllocationTable::Entry::Destroy()
{
    if (Next != nullptr)
    {
        Next->Destroy();
    }

    delete Next;
}

SigourneyEngine::FunctionalLayer::Memory::AllocationTable::~AllocationTable()
{
    if (Head == nullptr)
        return;

    Head->Destroy();
    delete Head;
}

SigourneyEngine::FunctionalLayer::Memory::AllocationTable::Entry* SigourneyEngine::FunctionalLayer::Memory::AllocationTable::Seek(size_t size)
{
    if (Head == nullptr)
    {
        Head = new Entry{ nullptr, size, 0 };
        Length++;
        return Head;
    }

    if (Head->Size == size)
        return Head;

    Entry* currentNode = Head;
    while (currentNode->Next != nullptr)
    {
        if (currentNode->Next->Size == size)
            return currentNode->Next;

        currentNode = currentNode->Next;
    }

    currentNode->Next = new Entry{ nullptr, size, currentNode->Index + 1 };
    Length++;
    return currentNode->Next;
}

SigourneyEngine::FunctionalLayer::Memory::AllocationTable* SigourneyEngine::FunctionalLayer::Memory::AllocationTable::GetSingleton()
{
    return &MetaTable;
}

unsigned int SigourneyEngine::FunctionalLayer::Memory::AllocationTable::RegisterType(size_t size)
{
    Entry* metaNode = MetaTable.Seek(size);
    return metaNode->Index;
}

unsigned int SigourneyEngine::FunctionalLayer::Memory::AllocationTable::GetTableLength()
{
    return MetaTable.Length;
}
