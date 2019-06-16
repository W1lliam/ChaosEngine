#pragma once

#include "ChaosCore/Memory/Allocators/AAllocator.h"

namespace Chaos::Memory
{
    class PoolAllocator : public AAllocator
    {
    public:
        PoolAllocator(size_t p_totalSize, size_t p_objectSize, uint8_t p_objectAlignment);
        ~PoolAllocator() override = default;
        void*   Allocate(size_t p_size, uint8_t p_alignment) override;
        void    Init() override;
        void    Free(void* p_pointer) override;
        void    Clear() override;

    private:
        const   uint8_t	    OBJECT_ALIGNMENT;
        const   size_t	    OBJECT_SIZE;
                void**      m_freeList{ nullptr };

    };
}
