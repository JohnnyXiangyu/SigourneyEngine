#pragma once
#include "Utils/shorthand_functions.h"
#include <memory>

namespace Engine {
namespace Core {
namespace Memory {

// single-linked list of free buffers
class HomogeneousStorage
{
private:
    /// <summary>
    /// Standard metadata header for each block in a buffer chain.
    /// </summary>
    struct Node
    {
        Node* NextFree = nullptr;

        // get the pointer to a payload
        void* GetPayload() { return ((char*)this) + sizeof(Node); }
    };

    /// <summary>
    /// A segment of a chain of buffers.
    /// </summary>
    struct Segment
    {
        Segment* NextSegment = nullptr;
        unsigned int Count = 0;
    };

    static Node* GetFirstHeaderFromSegment(Segment* segment)
    {
        return (Node*)Utils::SkipHeader<Segment>(segment);
    }

    static char* GetBufferFromHeader(Node* header)
    {
        return (char*)Utils::SkipHeader<Node>(header);
    }

    // creates a chain segment data structure, and a buffer immediately after it
    Segment* CreateNewSegment(unsigned int count, size_t payloadSize);

    // state starts here:

    Segment* m_headSegment = nullptr;
    Segment* m_tailSegment = nullptr;

    Node* m_headNode = nullptr;

    size_t m_payloadSize = 0;
    unsigned int m_initialCount = 0;

    // it does change internal state, not a const function
    void ResetSegment(Segment* newSegment);

public:
    HomogeneousStorage(size_t payloadSize, unsigned int initialCount) : m_payloadSize(payloadSize), m_initialCount(initialCount) {}
    ~HomogeneousStorage();

    void* Take();

    void Put(void* pointer);

    void Reset();
};

}
}
}
