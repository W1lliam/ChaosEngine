#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"
#include <ChaosDebug.h>


namespace Chaos::Memory
{
    PoolAllocator::PoolAllocator(const size_t p_totalSize, const size_t p_objectSize, const uint8_t p_objectAlignment) :
        AAllocator(p_totalSize),
        OBJECT_ALIGNMENT(p_objectAlignment),
        OBJECT_SIZE(p_objectSize)
    {
        Init();
    }

    void* PoolAllocator::Allocate(const size_t p_size, const uint8_t p_alignment)
    {
        CHAOS_CORE_ASSERT(p_size > 0, "allocate called with memSize = 0.");
        CHAOS_CORE_ASSERT(m_usedMemory + p_size <= m_totalSize, "Allocator full");
        CHAOS_CORE_ASSERT(p_size == this->OBJECT_SIZE && p_alignment == this->OBJECT_ALIGNMENT, "Pool allocator can only be used for specified object size and alignment");

        // Allocator not initialized
        if (!m_freeList)
            return nullptr;

        void* currentAddress = m_freeList;
        m_freeList = static_cast<void**>(*m_freeList);

        m_usedMemory += OBJECT_SIZE;
        m_peak = std::max(m_peak, m_usedMemory);
        ++m_allocationCount;

        return currentAddress;
    }

    void PoolAllocator::Init()
    {
        AAllocator::Init();
        if (!m_freeList)
            Clear();
    }

    void PoolAllocator::Free(void* p_pointer)
    {
        // Put this slot back to free list
        *static_cast<void**>(p_pointer) = m_freeList;

        m_freeList = static_cast<void**>(p_pointer);

        m_usedMemory -= OBJECT_SIZE;
        --m_allocationCount;
    }

    void PoolAllocator::Clear()
    {
        const size_t padding = CalculatePadding(m_basePtr.asUIntPtr, OBJECT_ALIGNMENT);
        const size_t objectsCount = (m_totalSize - padding) / OBJECT_SIZE;
        m_freeList = reinterpret_cast<void**>(m_basePtr.asUIntPtr + padding);

        void** currentAddress = m_freeList;

        for (size_t i = 0; i < (objectsCount - 1); ++i)
        {
            *currentAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(currentAddress) + OBJECT_SIZE);

            currentAddress = reinterpret_cast<void**>(*currentAddress);
        }

        *currentAddress = nullptr;

        m_peak = 0;
        m_usedMemory = 0;
        m_allocationCount = 0;
    }
}