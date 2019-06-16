#pragma once

#include "ChaosCore/Memory/Allocators/AAllocator.h"
#include "ChaosCore/Memory/Allocators/StackAllocator.h"

namespace Chaos::Memory
{
    class DoubleBufferedAllocator : public AAllocator
    {
    public:
        DoubleBufferedAllocator(size_t p_totalSize);
        ~DoubleBufferedAllocator() = default;

        void SwapBuffers();
        void ClearAllBuffers();

        virtual void * Allocate(size_t p_size, uint8_t p_alignment = 0) override;
        virtual void Free(void * p_pointer) override;
        virtual void Clear() override;

    private:
        StackAllocator  m_stack[2];
        uint8_t         m_currentStack  {0};
    };
}