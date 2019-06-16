#include "ChaosCore/chaos_core_pch.h"
#include "ChaosDebug.h"
#include "ChaosCore/Memory/Allocators/AAllocator.h"

namespace Chaos::Memory
{
    AAllocator::AAllocator(const size_t p_totalSize) : m_totalSize(p_totalSize)
    {
    }


    AAllocator::~AAllocator()
    {

        free(m_basePtr.asVoidPtr);
        m_basePtr.asVoidPtr = nullptr;
        m_totalSize = 0;
    }

    void AAllocator::Init()
    {
        if (m_basePtr.asVoidPtr == nullptr)
        {
            m_basePtr.asVoidPtr = malloc(m_totalSize);
        }
        else
        {
            Clear();
        }
        m_usedMemory = 0;

        if (m_basePtr.asVoidPtr == nullptr)
            throw std::bad_alloc();

    }
}