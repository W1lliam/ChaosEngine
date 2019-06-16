#pragma once

#include "ChaosCore/Memory/Allocators/AAllocator.h"

namespace Chaos::Memory
{
    class StackAllocator : public AAllocator
    {
    public:
        explicit    StackAllocator(size_t p_totalSize);
        ~StackAllocator() = default;

        void*       Allocate(size_t p_size, uint8_t p_alignment) override;
        void        Free(void* p_pointer) override;
        void        Clear() override;
    private:
        using Header = size_t;
    };
}
