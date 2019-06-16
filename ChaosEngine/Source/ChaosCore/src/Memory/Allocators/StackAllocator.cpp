#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Memory/Allocators/StackAllocator.h"
#include <ChaosDebug.h>


namespace Chaos::Memory
{
    StackAllocator::StackAllocator(const size_t p_totalSize) : AAllocator(p_totalSize)
    {
    }

    void* StackAllocator::Allocate(const size_t p_size, const uint8_t p_alignment)
    {
        CHAOS_CORE_ASSERT(p_size > 0, "Allocate called with size < 0");

        // Allocator not initialized
        if (!m_basePtr.asVoidPtr)
            return nullptr;

        u_memoryPtr<Header> currentAddress;

        currentAddress.asVoidPtr = m_basePtr.asVoidPtr;
        currentAddress.asUIntPtr += m_usedMemory;

        const size_t padding = CalculatePadding(currentAddress.asUIntPtr, p_alignment, sizeof(size_t));

        if (m_usedMemory + padding + p_size > m_totalSize)
        {
            // Not enough memory
            return nullptr;
        }

        *currentAddress.asHeader = padding;
        currentAddress.asUIntPtr += padding;

        m_usedMemory += p_size + padding;
        m_peak = std::max(m_peak, m_usedMemory);
        ++m_allocationCount;

        return currentAddress.asVoidPtr;
    }

    void StackAllocator::Free(void* p_pointer)
    {
        u_memoryPtr<Header> currentAddress;
        currentAddress.asVoidPtr = p_pointer;
        currentAddress.asUIntPtr -= sizeof(Header);

        m_usedMemory = m_basePtr.asUIntPtr - reinterpret_cast<uintptr_t>(p_pointer) - *currentAddress.asHeader;
        --m_allocationCount;
    }

    void StackAllocator::Clear()
    {
        m_peak = 0;
        m_usedMemory = 0;
        m_allocationCount = 0;
    }
}