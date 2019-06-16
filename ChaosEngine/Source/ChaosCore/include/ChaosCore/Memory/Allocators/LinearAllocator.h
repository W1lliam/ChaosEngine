#pragma once

#include "ChaosCore/Memory/Allocators/AAllocator.h"

namespace Chaos::Memory
{
    class LinearAllocator : public AAllocator
    {
    public:
        explicit   LinearAllocator(size_t p_totalSize);
        ~LinearAllocator() = default;

        void*      Allocate(size_t p_size, uint8_t p_alignment = 0) override;
        void       Free(void* p_pointer) override;
        void       Clear() override;

        LinearAllocator(const LinearAllocator&) = delete;
        LinearAllocator(LinearAllocator&&) = delete;
        LinearAllocator& operator=(const LinearAllocator&) = delete;
        LinearAllocator& operator=(LinearAllocator&&) = delete;
    };
}