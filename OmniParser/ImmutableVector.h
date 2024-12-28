#pragma once

#include <vector>
#include <memory>
#include <stdexcept>

namespace OmniParser
{
    template <typename T>
    struct ImmutableVectorBlock
    {
    public:
        std::shared_ptr<T> sourceMemory;

        // inclusive
        size_t startIndex;

        // exclusive
        size_t endIndex;

        size_t Length() const
        {
            return endIndex - startIndex;
        }

        const T& At(size_t index) const
        {
            return sourceMemory.get()[index];
        }

        ImmutableVectorBlock(std::shared_ptr<T> inMemory, size_t inStart, size_t inEnd)
        {
            sourceMemory = inMemory;
            startIndex = inStart;
            endIndex = inEnd;
        }
    };

    // use shared pointers to manage memory
    template <typename T>
    class ImmutableVector
    {
    private:
        std::vector<ImmutableVectorBlock<T>> m_blocks = std::vector<ImmutableVectorBlock<T>>();
        size_t m_length = 0;

        void AppendBlock(ImmutableVectorBlock<T>& newBlock)
        {
            m_blocks.push_back(newBlock);
            m_length += newBlock.Length();
        }

        // warning: mutation!
        size_t FindIndexInBlocks(size_t& index) const
        {
            // no need to check in a private function
            //if (index >= m_length)
            //    throw std::invalid_argument("argument out of range: index, ImmutableVector::FindIndexBlocks");
            int i = 0;
            for (; index >= m_blocks[i].Length() && i < m_blocks.size(); i++)
            {
                index -= m_blocks[i].Length();
            }
            return i;
        }

        ImmutableVector(const ImmutableVector<T>& other, size_t startBlock, size_t endBlock) : m_length(0)
        {
            if (startBlock >= other.m_blocks.size() || endBlock <= startBlock || endBlock > other.m_blocks.size())
                throw std::invalid_argument("invalid range arguments, ImmutableVector range constructor");

            m_blocks = std::vector<ImmutableVectorBlock<T>>(other.m_blocks.begin() + startBlock, other.m_blocks.begin() + endBlock);
            for (const ImmutableVectorBlock<T>& block : m_blocks)
            {
                m_length += block.Length();
            }
        }

        ImmutableVector(std::vector<ImmutableVectorBlock<T>>& storage)
        {
            m_blocks = storage;
            for (const ImmutableVectorBlock<T>& block : m_blocks)
            {
                m_length += block.Length();
            }
        }

    public:
        ImmutableVector() = default;

        ImmutableVector(const ImmutableVector<T>& other)
        {
            m_blocks = std::vector<ImmutableVectorBlock<T>>(other.m_blocks);
            m_length = other.m_length;
        }

        size_t Length() const
        {
            return m_length;
        }

        // less efficient, since you are not pulling 
        ImmutableVector Add(const T& item) const
        {
            // create a new block to hold that one item, adjust the length
            ImmutableVector<T> newVector = ImmutableVector<T>(*this);
            newVector.m_blocks.push_back(ImmutableVectorBlock<T>(std::shared_ptr<T>(new T(item)), 0, 1));
            newVector.m_length++;
            return newVector;
        }

        ImmutableVector AppendCopy(T* newItems, T* end) const
        {
            size_t length = end - newItems;
            std::shared_ptr<T> newStorage = std::shared_ptr<T>(new T[length]);
            for (int i = 0; i < length; i++)
            {
                newStorage.get()[i] = newItems[i];
            }

            ImmutableVectorBlock<T> newBlock = ImmutableVectorBlock<T>(newStorage, 0, length);

            ImmutableVector newVector = ImmutableVector(*this);
            newVector.AppendBlock(newBlock);

            return newVector;
        }

        ImmutableVector AppendRaw(std::shared_ptr<T> storage, size_t start, size_t end) const
        {
            ImmutableVector newVector = ImmutableVector(*this);
            ImmutableVectorBlock<T> newBlock = ImmutableVectorBlock<T>(storage, start, end);
            newVector.AppendBlock(newBlock);
            return newVector;
        }

        const T& At(size_t index) const
        {
            if (index >= m_length)
                throw std::invalid_argument("argument too long: index, ImmutableVector::ItemAt");

            for (const ImmutableVectorBlock<T>& block : m_blocks)
            {
                if (index < block.Length())
                    return block.At(index);

                index -= block.Length();
            }

            throw std::runtime_error("unexpected code path, ImmutableVector::ItemAt");
        }

        ImmutableVector Take(size_t count) const
        {
            if (count == m_length)
                return *this;

            if (count == 0)
                return ImmutableVector<T>();

            if (count > m_length)
                throw std::runtime_error("argument out of range: count, Immutablevector::Head");

            int targetBlock = FindIndexInBlocks(count);
            ImmutableVector newVector(*this, 0, targetBlock);

            ImmutableVectorBlock<T> newBlock(m_blocks[targetBlock].sourceMemory, 0, count);

            newVector.AppendBlock(newBlock);
            return newVector;
        }

        ImmutableVector Skip(size_t headLength) const
        {
            if (headLength == m_length)
                return *this;

            if (headLength == 0)
                return ImmutableVector<T>();

            if (headLength > m_length)
                throw std::runtime_error("argument out of range: count, Immutablevector::Tail");

            int targetBlock = FindIndexInBlocks(headLength);
            
            ImmutableVector newVector(*this, targetBlock, m_blocks.size());
            newVector.m_blocks[0].startIndex += headLength;

            return newVector;
        }

        ImmutableVector Concat(const ImmutableVector& other)
        {
            ImmutableVector newVector = ImmutableVector();

            for (const ImmutableVectorBlock<T>& block : m_blocks)
            {
                newVector.m_blocks.push_back(block);
                newVector.m_length += block.Length();
            }

            for (const ImmutableVectorBlock<T>& block : other.m_blocks)
            {
                newVector.m_blocks.push_back(block);
                newVector.m_length += block.Length();
            }
        }
    };
}
