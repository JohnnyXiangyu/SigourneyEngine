#include "homogeneous_storage.h"

// creates a chain segment data structure, and a buffer immediately after it

using namespace Engine::Core::Memory;

HomogeneousStorage::Segment* HomogeneousStorage::CreateNewSegment(unsigned int count, size_t payloadSize)
{
    // create a buffer whose size is the required amount of nodes PLUS a segment header
    size_t nodeSize = payloadSize + sizeof(Node);
    size_t chainSize = nodeSize * count;
    char* newBuffer = new char[chainSize + sizeof(Segment)];

    // initialize the first address into a header
    Segment* newSegment = new (newBuffer) Segment;
    newSegment->Count = count;

    ResetSegment(newSegment);

    // header is at the beginning of the new memory range
    return newSegment;
}


// it does change internal state, not a const function

void HomogeneousStorage::ResetSegment(Segment* newSegment)
{
    char* newBuffer = (char*)newSegment;
    size_t nodeSize = m_payloadSize + sizeof(Node);

    // initialize first n-1 nodes
    char* nodesBegin = newBuffer + sizeof(Segment);
    for (unsigned int i = 0; i < newSegment->Count - 1; i++)
    {
        Node* currentNode = (Node*)(nodesBegin + nodeSize * i);
        Node* nextNode = (Node*)(nodesBegin + nodeSize * i + nodeSize);
        currentNode->NextFree = nextNode;
    }

    // initialize the last node (idk, it's good to avoid having lots of if-statements?)
    Node* lastNode = (Node*)(nodesBegin + nodeSize * newSegment->Count - nodeSize);
    lastNode->NextFree = nullptr;
}


HomogeneousStorage::~HomogeneousStorage()
{
    // increment to the next segment then delete the current
    // delete is called on the BufferChainSegment* 
    Segment* nextSeg = m_headSegment;
    while (nextSeg != nullptr)
    {
        Segment* currentSeg = nextSeg;
        nextSeg = nextSeg->NextSegment;
        delete[] currentSeg;
    }
}


void* HomogeneousStorage::Take()
{
    // initialize the first segment
    if (m_headSegment == nullptr)
    {
        // allocate a new segment and initialize the chain on top level
        m_headSegment = CreateNewSegment(m_initialCount, m_payloadSize);
        m_tailSegment = m_headSegment;

        // the new segment always supplies its first free node at the beginning of its buffer
        m_headNode = GetFirstHeaderFromSegment(m_headSegment);
    }

    // extend the list
    if (m_headNode == nullptr)
    {
        Segment* newSegment = CreateNewSegment(m_tailSegment->Count * 2, m_payloadSize);
        m_tailSegment->NextSegment = newSegment;
        m_tailSegment = newSegment;
        m_headNode = GetFirstHeaderFromSegment(m_tailSegment);
    }

    // increment head and return the previous head
    void* result = m_headNode->GetPayload();
    m_headNode = m_headNode->NextFree;
    return result;
}


void Engine::Core::Memory::HomogeneousStorage::Put(void* pointer)
{
    Node* header = Engine::Core::Utils::GetHeader<Node>(pointer);
    header->NextFree = m_headNode;
    m_headNode = header;
}


void HomogeneousStorage::Reset()
{
    for (Segment* segment = m_headSegment; segment != nullptr; segment = segment->NextSegment)
    {
        ResetSegment(segment);
    }

    m_headNode = GetFirstHeaderFromSegment(m_headSegment);
}
