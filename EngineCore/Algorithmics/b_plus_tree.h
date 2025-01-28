#pragma once

namespace SigourneyEngine
{
namespace Core
{

namespace Memory { class HighIntegrityAllocator; }

namespace EntityComponentSystem
{

struct BPlusNodeSegment
{
	void* SmallerOrPayload = nullptr;
	int Key = 0;
};

/// <summary>
/// What if we ... assume K = 3?
/// </summary>
struct BPlusNode
{
	BPlusNodeSegment Segment1;
	BPlusNodeSegment Segment2;
	BPlusNodeSegment Segment3;
	BPlusNode* Next = nullptr;
	unsigned int Depth = 0;
};

/// <summary>
/// Fast index tree: currently only support integer indexing. Payload value = void pointer.
/// </summary>
struct BPlusTree
{
	Memory::HighIntegrityAllocator* Allocator = nullptr;
	BPlusNode* Top = nullptr;
	BPlusNode* Head = nullptr;
	unsigned int TotalDepth = 0;


};

}
}
}
