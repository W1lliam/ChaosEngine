#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Memory/Allocators/DoubleBufferedAllocator.h"

namespace Chaos::Memory
{
    DoubleBufferedAllocator::DoubleBufferedAllocator(size_t p_totalSize) : 
        AAllocator(p_totalSize),
        m_stack{ StackAllocator(p_totalSize), StackAllocator(p_totalSize) }
    {
    }

    void DoubleBufferedAllocator::SwapBuffers()
    {
        m_currentStack ^= 1UL;
    }

    void DoubleBufferedAllocator::ClearAllBuffers()
    {
        m_stack[0].Clear();
        m_stack[1].Clear();
    }

    void* DoubleBufferedAllocator::Allocate(size_t p_size, uint8_t p_alignment)
    {
        return m_stack[m_currentStack].Allocate(p_size, p_alignment);
    }

    void DoubleBufferedAllocator::Free(void * p_pointer)
    {
        return m_stack[m_currentStack].Free(p_pointer);
    }

    void DoubleBufferedAllocator::Clear()
    {
        m_stack[m_currentStack].Clear();
    }
}