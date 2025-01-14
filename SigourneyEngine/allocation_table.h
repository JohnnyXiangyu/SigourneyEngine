#pragma once

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Memory {

class AllocationTable
{
private:
    // RTTI and statically collected information:
    struct Entry
    {
        Entry* Next;
        size_t Size;
        unsigned int Index;

        void Destroy();
    };

    unsigned int Length = 0;
    Entry* Head = nullptr;

    Entry* Seek(size_t size);

public:
    ~AllocationTable();

    static AllocationTable* GetSingleton();

    unsigned int RegisterType(size_t size);

    unsigned int GetTableLength();
};

}
}
}
