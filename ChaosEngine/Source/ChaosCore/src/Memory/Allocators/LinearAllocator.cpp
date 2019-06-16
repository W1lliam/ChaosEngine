#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Memory/Allocators/LinearAllocator.h"
#include <ChaosDebug.h>

namespace Chaos::Memory
{
    LinearAllocator::LinearAllocator(const size_t p_totalSize) : AAllocator(p_totalSize)
    {
    }

    void* LinearAllocator::Allocate(const size_t p_size, const uint8_t p_alignment)
    {
        CHAOS_CORE_ASSERT(p_size > 0, "Allocate called with size < 0");

        // Allocator not initialized
        if (!m_basePtr.asVoidPtr)
            return nullptr;

        u_memoryPtr<void> currentAddress;


        currentAddress.asVoidPtr = m_basePtr.asVoidPtr;
        currentAddress.asUIntPtr += m_usedMemory;

        const size_t padding = CalculatePadding(currentAddress.asUIntPtr, p_alignment);

        if (m_usedMemory + padding + p_size > m_totalSize)
        {
            // Not enough memory
            return nullptr;
        }

        m_usedMemory += p_size + padding;
        m_peak = std::max(m_peak, m_usedMemory);
        currentAddress.asUIntPtr += padding;
        ++m_allocationCount;
        return currentAddress.asVoidPtr;
    }

    void LinearAllocator::Free(void* p_pointer)
    {
        CHAOS_CORE_ASSERT(false, "Cannot free linear allocated variable, Use Clear() instead");
    }

    void LinearAllocator::Clear()
    {
        m_peak = 0;
        m_usedMemory = 0;
        m_allocationCount = 0;
    }
}