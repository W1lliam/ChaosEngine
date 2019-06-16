#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Memory/Allocators/TLSFAllocator.h"

namespace Chaos::Memory
{

    const size_t TLSFAllocator::BlockHeader::BLOCK_START_OFFSET {offsetof(TLSFAllocator::BlockHeader, size) + sizeof(size_t)};
    const size_t TLSFAllocator::BlockHeader::BLOCK_MIN_SIZE     {sizeof(TLSFAllocator::BlockHeader) - sizeof(TLSFAllocator::BlockHeader*)};

    TLSFAllocator::TLSFAllocator(size_t p_totalSize) : AAllocator(p_totalSize)
    {
        CHAOS_CORE_ASSERT(p_totalSize > TLSF_HEADER_SIZE + POOL_OVERHEAD + BlockHeader::BLOCK_MIN_SIZE, "Desired size is too small for TLSF overhead memory");
    }

    TLSFAllocator::~TLSFAllocator()
    {
    }

    void TLSFAllocator::Init()
    {
        CHAOS_CORE_ASSERT(TestFfsFls(), "Find first set or find last set is not correctly implemented !");
        AAllocator::Init();
        if ((m_basePtr.asUIntPtr % ALIGN_SIZE) != 0)
        {
            throw std::exception();
        }

        reinterpret_cast<TLSFHeader*>(m_basePtr.asVoidPtr)->Init();
        AddPool(reinterpret_cast<void*>(m_basePtr.asUIntPtr + TLSF_HEADER_SIZE), m_totalSize - TLSF_HEADER_SIZE);
    }

    void* TLSFAllocator::Allocate(size_t p_size, uint8_t p_alignment)
    {
        u_memoryPtr<TLSFHeader> tlfs;
        tlfs.asVoidPtr = m_basePtr.asVoidPtr;

        CHAOS_CORE_ASSERT(!(p_alignment % ALIGN_SIZE), "alignment should be a multiple of 8");
        const size_t adjust = AdjustRequestSize(p_size, p_alignment);
        BlockHeader* block = LocateFreeBlock(adjust);

        m_usedMemory += adjust;
        m_peak = std::max(m_peak, m_usedMemory);
        ++m_allocationCount;

        return PrepareUsedBlock(block, adjust);
    }

    void TLSFAllocator::Free(void* p_pointer)
    {
        if (p_pointer)
        {
            BlockHeader* block = BlockHeader::FromPointer(p_pointer);
            CHAOS_CORE_ASSERT(!block->IsFree(), "block already marked as free");
            block->MarkAsFree();

            m_usedMemory -= block->GetSize();
            --m_allocationCount;

            block = MergePrevBlock(block);
            block = MergeNextBlock(block);
            InsertBlock(block);
        }
    }

    void TLSFAllocator::Clear()
    {
        m_peak = 0;
        m_usedMemory = 0;
        m_allocationCount = 0;
        reinterpret_cast<TLSFHeader*>(m_basePtr.asVoidPtr)->Init();
        AddPool(reinterpret_cast<void*>(m_basePtr.asUIntPtr + TLSF_HEADER_SIZE), m_totalSize - TLSF_HEADER_SIZE);
    }

    void TLSFAllocator::AddPool(void* p_memory, size_t p_poolSize)
    {
        BlockHeader* block;
        BlockHeader* next;

        const size_t poolSize = GetPreviousAlignedAddress(p_poolSize - POOL_OVERHEAD, ALIGN_SIZE);

        if (((uintptr_t)p_memory % ALIGN_SIZE) != 0)
        {
            throw std::exception();
        }

        if (poolSize < BlockHeader::BLOCK_MIN_SIZE || poolSize > BlockHeader::BLOCK_MAX_SIZE)
        {
            throw std::exception();
        }

        /*
        ** Create the main free block. Offset the start of the block slightly
        ** so that the prevPhysBlock field falls outside of the pool -
        ** it will never be used.
        */
        block = BlockHeader::OffsetToNext(p_memory, -(int)BlockHeader::BLOCK_HEADER_OVERHEAD);
        block->SetSize(poolSize);
        block->SetFree();
        block->SetPrevUsed();
        InsertBlock(block);

        /* Split the block to create a zero-size sentinel block. */
        next = block->LinkNext();
        next->SetSize(0);
        next->SetUsed();
        next->SetPrevFree();
    }

    constexpr size_t TLSFAllocator::AdjustRequestSize(size_t p_size, size_t p_alignment)
    {
        size_t adjust = 0;
        if (p_size && p_size < BlockHeader::BLOCK_MAX_SIZE)
        {
            const size_t aligned = GetNextAlignedAddress(p_size, p_alignment);
            adjust = std::max(aligned, BlockHeader::BLOCK_MIN_SIZE);
        }
        return adjust;
    }

    void TLSFAllocator::MappingInsert(size_t p_size, int& o_flIndex, int& o_slIndex)
    {
        o_flIndex = 0;
        o_slIndex = 0;
        if (p_size < SMALL_BLOCK_SIZE)
        {
            /* Store small blocks in first list. */
            o_flIndex = 0;
            o_slIndex = static_cast<int>(p_size) / (SMALL_BLOCK_SIZE / SL_INDEX_COUNT);
        }
        else
        {
            o_flIndex = FindLastSet_sizet(p_size);
            o_slIndex = static_cast<int>(p_size >> (o_flIndex - SL_INDEX_COUNT_LOG2)) ^ (1 << SL_INDEX_COUNT_LOG2);
            o_flIndex -= (FL_INDEX_SHIFT - 1);
        }
    }

    void TLSFAllocator::MappingSearch(size_t p_size, int& o_flIndex, int& o_slIndex)
    {
        if (p_size >= SMALL_BLOCK_SIZE)
        {
            const size_t round = (1 << (FindLastSet_sizet(p_size) - SL_INDEX_COUNT_LOG2)) - 1;
            p_size += round;
        }
        MappingInsert(p_size, o_flIndex, o_slIndex);
    }

    TLSFAllocator::BlockHeader* TLSFAllocator::SearchSuitableBlock(int& o_flIndex, int& o_slIndex)
    {
        int fl = o_flIndex;
        int sl = o_slIndex;

        u_memoryPtr<TLSFHeader> tlfs;
        tlfs.asVoidPtr = m_basePtr.asVoidPtr;

        /*
        ** First, search for a block in the list associated with the given
        ** fl/sl index.
        */
        unsigned int slMap = tlfs.asHeader->slBitmap[fl] & (~0U << sl);
        if (!slMap)
        {
            /* No block exists. Search in the next largest first-level list. */
            const unsigned int flMap = tlfs.asHeader->flBitmap & (~0U << (fl + 1));
            if (!flMap)
            {
                /* No free blocks available, memory has been exhausted. */
                return 0;
            }

            fl = FindFirstSet(flMap);
            o_flIndex = fl;
            slMap = tlfs.asHeader->slBitmap[fl];
        }
        CHAOS_CORE_ASSERT(slMap, "internal error - second level bitmap is null");
        sl = FindFirstSet(slMap);
        o_slIndex = sl;

        /* Return the first block in the free list. */
        return tlfs.asHeader->blocks[fl][sl];
    }

    void TLSFAllocator::RemoveFreeBlock(BlockHeader* p_block, int p_flIndex, int slIndex)
    {
        BlockHeader* prev = p_block->prevFreeBlock;
        BlockHeader* next = p_block->nextFreeBlock;
        CHAOS_CORE_ASSERT(prev, "prev_free field can not be null");
        CHAOS_CORE_ASSERT(next, "next_free field can not be null");
        u_memoryPtr<TLSFHeader> tlfs;
        tlfs.asVoidPtr = m_basePtr.asVoidPtr;

        next->prevFreeBlock = prev;
        prev->nextFreeBlock = next;

        /* If this block is the head of the free list, set new head. */
        if (tlfs.asHeader->blocks[p_flIndex][slIndex] == p_block)
        {
            tlfs.asHeader->blocks[p_flIndex][slIndex] = next;

            /* If the new head is null, clear the bitmap. */
            if (next == &tlfs.asHeader->nullBlock)
            {
                tlfs.asHeader->slBitmap[p_flIndex] &= ~(1 << slIndex);

                /* If the second bitmap is now empty, clear the fl bitmap. */
                if (!tlfs.asHeader->slBitmap[p_flIndex])
                {
                    tlfs.asHeader->flBitmap &= ~(1 << p_flIndex);
                }
            }
        }
    }

    void TLSFAllocator::InsertFreeBlock(BlockHeader* p_block, int p_flIndex, int slIndex)
    {
        u_memoryPtr<TLSFHeader> tlfs;
        tlfs.asVoidPtr = m_basePtr.asVoidPtr;

        BlockHeader* current = tlfs.asHeader->blocks[p_flIndex][slIndex];
        CHAOS_CORE_ASSERT(current, "free list cannot have a null entry");
        CHAOS_CORE_ASSERT(p_block, "cannot insert a null entry into the free list");
        p_block->nextFreeBlock = current;
        p_block->prevFreeBlock = &tlfs.asHeader->nullBlock;
        current->prevFreeBlock = p_block;

        CHAOS_CORE_ASSERT(BlockHeader::ToPointer(p_block) == GetNextAlignedAddress(BlockHeader::ToPointer(p_block), ALIGN_SIZE), "block not aligned properly");
        /*
        ** Insert the new block at the head of the list, and mark the first-
        ** and second-level bitmaps appropriately.
        */
        tlfs.asHeader->blocks[p_flIndex][slIndex] = p_block;
        tlfs.asHeader->flBitmap |= (1 << p_flIndex);
        tlfs.asHeader->slBitmap[p_flIndex] |= (1 << slIndex);
    }

    void TLSFAllocator::RemoveBlock(BlockHeader* p_block)
    {
        int flIndex, slIndex;
        MappingInsert(p_block->GetSize(), flIndex, slIndex);
        RemoveFreeBlock(p_block, flIndex, slIndex);
    }

    void TLSFAllocator::InsertBlock(BlockHeader* p_block)
    {
        int fl, sl;
        MappingInsert(p_block->GetSize(), fl, sl);
        InsertFreeBlock(p_block, fl, sl);
    }

    TLSFAllocator::BlockHeader* TLSFAllocator::SplitBlock(BlockHeader* p_block, size_t p_size)
    {
        /* Calculate the amount of space left in the remaining block. */
        BlockHeader* remaining = BlockHeader::OffsetToNext(BlockHeader::ToPointer(p_block), p_size - BlockHeader::BLOCK_HEADER_OVERHEAD);

        const size_t remainSize = p_block->GetSize() - (p_size + BlockHeader::BLOCK_HEADER_OVERHEAD);

        CHAOS_CORE_ASSERT(BlockHeader::ToPointer(remaining) == GetNextAlignedAddress(BlockHeader::ToPointer(remaining), ALIGN_SIZE), "remaining block not aligned properly");

        CHAOS_CORE_ASSERT(p_block->GetSize() == remainSize + p_size + BlockHeader::BLOCK_HEADER_OVERHEAD, "Block size is too small to store block header");
        remaining->SetSize(remainSize);
        CHAOS_CORE_ASSERT(remaining->GetSize() >= BlockHeader::BLOCK_MIN_SIZE, "block split with invalid size");

        p_block->SetSize(p_size);
        remaining->MarkAsFree();

        return remaining;
    }

    TLSFAllocator::BlockHeader* TLSFAllocator::AbsorbBlock(BlockHeader* p_prev, BlockHeader* p_block)
    {
        CHAOS_CORE_ASSERT(!p_prev->IsLast(), "Previous block can't be last");
        p_prev->size += p_block->GetSize() + BlockHeader::BLOCK_HEADER_OVERHEAD;
        p_prev->LinkNext();
        return p_prev;
    }

    TLSFAllocator::BlockHeader* TLSFAllocator::MergePrevBlock(BlockHeader* p_block)
    {
        if (p_block->IsPrevFree())
        {
            BlockHeader* prev = p_block->Previous();
            CHAOS_CORE_ASSERT(prev, "Previous physical block can't be null");
            CHAOS_CORE_ASSERT(prev->IsFree(), "Previous physical block is not free but marked as such in current block");
            RemoveBlock(prev);
            p_block = AbsorbBlock(prev, p_block);
        }

        return p_block;
    }

    TLSFAllocator::BlockHeader* TLSFAllocator::MergeNextBlock(BlockHeader* p_block)
    {
        BlockHeader* next = p_block->Next();
        CHAOS_CORE_ASSERT(next, "next physical block can't be null");

        if (next->IsFree())
        {
            CHAOS_CORE_ASSERT(!p_block->IsLast(), "Previous block can't be last");
            RemoveBlock(next);
            p_block = AbsorbBlock(p_block, next);
        }

        return p_block;
    }

    void TLSFAllocator::TrimFreeBlock(BlockHeader* p_block, size_t p_size)
    {
        CHAOS_CORE_ASSERT(p_block->IsFree(), "block must be free");
        if (p_block->CanSplit(p_size))
        {
            BlockHeader* remainingBlock = SplitBlock(p_block, p_size);
            p_block->LinkNext();
            remainingBlock->SetPrevFree();
            InsertBlock(remainingBlock);
        }
    }

    TLSFAllocator::BlockHeader* TLSFAllocator::LocateFreeBlock(size_t p_size)
    {
        int fl = 0, sl = 0;
        BlockHeader* block = 0;

        if (p_size)
        {
            MappingSearch(p_size, fl, sl);

            /*
            ** MappingSearch can futz with the size, so for excessively large sizes it can sometimes wind up
            ** with indices that are off the end of the block array.
            ** So, we protect against that here, since this is the only callsite of MappingSearch.
            ** Note that we don't need to check sl, since it comes from a modulo operation that guarantees it's always in range.
            */
            if (fl < FL_INDEX_COUNT)
            {
                block = SearchSuitableBlock(fl, sl);
            }
        }

        if (block)
        {
            CHAOS_CORE_ASSERT(block->GetSize() >= p_size, "the block found is too small");
            RemoveFreeBlock(block, fl, sl);
        }

        return block;
    }
    void* TLSFAllocator::PrepareUsedBlock(BlockHeader* p_block, size_t p_size)
    {
        void* p = 0;
        if (p_block)
        {
            CHAOS_CORE_ASSERT(p_size, "size must be non-zero");
            TrimFreeBlock(p_block, p_size);
            p_block->MarkAsUsed();
            p = BlockHeader::ToPointer(p_block);
        }
        return p;
    }
}