#pragma once

#include "ChaosCore/Memory/Allocators/AAllocator.h"
#include "ChaosCore/Memory/Utils.h"
#include <ChaosDebug.h>


namespace Chaos::Memory
{
    class TLSFAllocator : public AAllocator
    {
    private:
        struct BlockHeader;
    public:
        /*
        ** log2 of number of linear subdivisions of block sizes.
        ** Larger values require more memory in the control structure.
        ** Values of 4 (16 subdivs) or 5 (32 subdivs) are typical.
        */
        static constexpr uint8_t SL_INDEX_COUNT_LOG2 = 5;
    public: 
        TLSFAllocator(size_t p_totalSize);
        ~TLSFAllocator();

        virtual void    Init() override;
        virtual void*   Allocate(size_t p_size, uint8_t p_alignment = 0) override;
        virtual void    Free(void * p_pointer) override;
        virtual void    Clear() override;

        void AddPool(void* p_memory, size_t p_poolSize);

    private:
        static constexpr size_t AdjustRequestSize(size_t p_size, size_t p_alignment);

        void            MappingInsert(size_t p_size, int& o_flIndex, int& o_slIndex);
        void            MappingSearch(size_t p_size, int& o_flIndex, int& o_slIndex);
        BlockHeader*    SearchSuitableBlock(int& o_flIndex, int& o_slIndex);

        void            RemoveFreeBlock(BlockHeader* p_block, int p_flIndex, int slIndex);
        void            InsertFreeBlock(BlockHeader* p_block, int p_flIndex, int slIndex);

        void            RemoveBlock(BlockHeader* p_block);
        void            InsertBlock(BlockHeader* p_block);

        BlockHeader*    SplitBlock(BlockHeader* p_block, size_t p_size);
        BlockHeader*    AbsorbBlock(BlockHeader* p_prev, BlockHeader* p_block);

        BlockHeader*    MergePrevBlock(BlockHeader* p_block);
        BlockHeader*    MergeNextBlock(BlockHeader* p_block);
        void            TrimFreeBlock(BlockHeader* p_block, size_t p_size);
        BlockHeader*    LocateFreeBlock(size_t p_size);
        void*           PrepareUsedBlock(BlockHeader* p_block, size_t p_size);

#if !defined(CHAOS_SHIPPING)
        static bool TestFfsFls()
        {
            /* Verify ffs/fls work properly. */
            int rv = 0;
            rv += (FindFirstSet(0) == -1) ? 0 : 0x1;
            rv += (FindLastSet(0) == -1) ? 0 : 0x2;
            rv += (FindFirstSet(1) == 0) ? 0 : 0x4;
            rv += (FindLastSet(1) == 0) ? 0 : 0x8;

            rv += (FindFirstSet(0x80000000) == 31) ? 0 : 0x10;
            rv += (FindFirstSet(0x80008000) == 15) ? 0 : 0x20;
            rv += (FindLastSet(0x80000008) == 31) ? 0 : 0x40;
            rv += (FindLastSet(0x7FFFFFFF) == 30) ? 0 : 0x80;

            rv += (FindLastSet_sizet(0x80000000) == 31) ? 0 : 0x100;
            rv += (FindLastSet_sizet(0x100000000) == 32) ? 0 : 0x200;
            rv += (FindLastSet_sizet(0xffffffffffffffff) == 63) ? 0 : 0x400;

            if (rv)
            {
                printf("TestFfsFls: %x ffs/fls tests failed.\n", rv);
            }
            return rv == 0;
        }
#endif

    private :

        //8 bytes alignment
        static constexpr uint8_t ALIGN_SIZE_LOG2        {3};
        static constexpr uint8_t ALIGN_SIZE             {1 << ALIGN_SIZE_LOG2};

        /*
        ** Allocations can go up to (1 << FL_INDEX_MAX) bits.
        ** However, because we linearly subdivide the second-level lists, and
        ** our minimum size granularity is 4 bytes, it doesn't make sense to
        ** create first-level lists for sizes smaller than SL_INDEX_COUNT * 4,
        ** or (1 << (SL_INDEX_COUNT_LOG2 + 2)) bytes, as there we will be
        ** trying to split size ranges into more slots than we have available.
        ** Instead, we calculate the minimum threshold size, and place all
        ** blocks below that size into the 0th first-level list.
        */
        static constexpr uint8_t    FL_INDEX_MAX        {32};
        static constexpr uint32_t   SL_INDEX_COUNT      {1 << SL_INDEX_COUNT_LOG2};

        static constexpr uint8_t    FL_INDEX_SHIFT      {SL_INDEX_COUNT_LOG2 + ALIGN_SIZE_LOG2};
        static constexpr uint8_t    FL_INDEX_COUNT      {FL_INDEX_MAX - FL_INDEX_SHIFT + 1};

        static constexpr uint32_t   SMALL_BLOCK_SIZE    {1 << FL_INDEX_SHIFT};


        static_assert(sizeof(int)    * CHAR_BIT == 32);
        static_assert(sizeof(size_t) * CHAR_BIT >= 32);
        static_assert(sizeof(size_t) * CHAR_BIT <= 64);

        /* SL_INDEX_COUNT must be <= number of bits in sl_bitmap's storage type. */
        static_assert(sizeof(unsigned int) * CHAR_BIT >= SL_INDEX_COUNT);

        /* Ensure we've properly tuned our sizes. */
        static_assert(ALIGN_SIZE == SMALL_BLOCK_SIZE / SL_INDEX_COUNT);

        struct BlockHeader
        {
            /* 
            ** Points to the previous physical block.
            ** The prevPhysBlock field is only valid if the previous block is free.
            ** The prevPhysBlock field is actually stored at the end of the previous block. 
            ** It appears at the beginning of this structure only to simplify the implementation.
            */
            BlockHeader* prevPhysBlock;
            size_t       size;
            /* The nextFreeBlock / prevFreeBlock fields are only valid if the block is free. */
            BlockHeader* nextFreeBlock;
            BlockHeader* prevFreeBlock;

            /*
            ** Since block sizes are always a multiple of 8, the two least
            ** significant bits of the size field are used to store the block status:
            ** - bit 0: whether block is busy or free
            ** - bit 1: whether previous block is busy or free
            */
            static constexpr size_t BLOCK_HEADER_FREE_BIT        {1 << 0};
            static constexpr size_t BLOCK_HEADER_PREV_FREE_BIT   {1 << 1};

            /*
            ** The size of the block header exposed to used blocks is the size field.
            ** The prevPhysBlock field is stored *inside* the previous free block.
            */
            static constexpr size_t BLOCK_HEADER_OVERHEAD   {sizeof(size_t)};

            /* User data starts directly after the size field in a used block. */
            static const     size_t BLOCK_START_OFFSET;

            /*
            ** A free block must be large enough to store its header minus the size of
            ** the prevPhysBlock field, and no larger than the number of addressable
            ** bits for FL_INDEX.
            */
            static const     size_t BLOCK_MIN_SIZE;
            static constexpr size_t BLOCK_MAX_SIZE  {(size_t)(1) << FL_INDEX_MAX};

            static inline       BlockHeader*   FromPointer(const void* p_pointer)                  { return reinterpret_cast<BlockHeader*>(reinterpret_cast<uintptr_t>(p_pointer) - BLOCK_START_OFFSET); }
            static inline       void*          ToPointer(const BlockHeader* p_pointer)             { return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(p_pointer) + BLOCK_START_OFFSET); }
            static constexpr    BlockHeader*   OffsetToNext(const void* p_pointer, size_t p_size)  { return reinterpret_cast<BlockHeader*>(reinterpret_cast<uintptr_t>(p_pointer) + p_size); }

            constexpr   int             CanSplit(size_t size)   { return GetSize() >= sizeof(BlockHeader) + size; }
            constexpr   size_t          GetSize()               { return size & ~(BLOCK_HEADER_FREE_BIT | BLOCK_HEADER_PREV_FREE_BIT); }
                        void            SetSize(size_t p_size)  { size = p_size | (size & (BLOCK_HEADER_FREE_BIT | BLOCK_HEADER_PREV_FREE_BIT)); }

            constexpr   bool            IsLast()                { return GetSize() == 0; }

            constexpr   bool            IsFree()                { return size & BLOCK_HEADER_FREE_BIT; }
            constexpr   void            SetFree()               { size |= BLOCK_HEADER_FREE_BIT; }
            constexpr   void            SetUsed()               { size &= ~BLOCK_HEADER_FREE_BIT; }

            constexpr   bool            IsPrevFree()            { return size & BLOCK_HEADER_PREV_FREE_BIT; }
            constexpr   void            SetPrevFree()           { size |= BLOCK_HEADER_PREV_FREE_BIT; }
            constexpr   void            SetPrevUsed()           { size &= ~BLOCK_HEADER_PREV_FREE_BIT; }

            constexpr   BlockHeader*    Previous()              { CHAOS_CORE_ASSERT(IsPrevFree(), "Previous block must be free"); return prevPhysBlock; }
                        BlockHeader*    Next()                  { CHAOS_CORE_ASSERT(!IsLast(), "Last block cannot have a next block"); return OffsetToNext(ToPointer(this), GetSize() - BLOCK_HEADER_OVERHEAD); }


            BlockHeader* LinkNext() 
            {    
                BlockHeader* next = Next();
                next->prevPhysBlock = this;
                return next;
            }

            void MarkAsFree() 
            {
                BlockHeader* next = LinkNext();
                next->SetPrevFree();
                SetFree();
            }

            void MarkAsUsed() 
            {
                BlockHeader* next = Next();
                next->SetPrevUsed();
                SetUsed();
            }
        };

        struct TLSFHeader
        {
            /* Empty lists point at this block to indicate they are free. */
            BlockHeader nullBlock;

            /* Bitmaps for free lists. */
            uint32_t flBitmap;
            uint32_t slBitmap[FL_INDEX_COUNT];

            /* Head of free lists. */
            BlockHeader* blocks[FL_INDEX_COUNT][SL_INDEX_COUNT];

            void Init()
            {
                int i, j;

                nullBlock.nextFreeBlock = &nullBlock;
                nullBlock.prevFreeBlock = &nullBlock;

                flBitmap = 0;
                for (i = 0; i < FL_INDEX_COUNT; ++i)
                {
                    slBitmap[i] = 0;
                    for (j = 0; j < SL_INDEX_COUNT; ++j)
                    {
                        blocks[i][j] = &nullBlock;
                    }
                }
            }
        };

        static constexpr size_t POOL_OVERHEAD       {2 * BlockHeader::BLOCK_HEADER_OVERHEAD};
        static constexpr size_t TLSF_HEADER_SIZE    {sizeof(TLSFHeader)};
    };


}