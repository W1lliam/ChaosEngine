#pragma once


namespace Chaos::Memory
{
	template<typename T>
	union u_memoryPtr
	{
		void*       asVoidPtr{ nullptr };
		uintptr_t   asUIntPtr;
		T*          asHeader;
	};

    class AAllocator
    {
    public:
        explicit        AAllocator(size_t p_totalSize);
        virtual         ~AAllocator();

        virtual void    Init();
        virtual void*   Allocate(size_t p_size, uint8_t p_alignment = 0) = 0;
        virtual void    Free(void* p_pointer) = 0;
        virtual void    Clear() = 0;

    protected:
        static void* GetNextAlignedAddress(const void* p_baseAddress, const uint8_t p_alignment)
        {
            return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(p_baseAddress) + (p_alignment - 1) & ~(p_alignment - 1));
        }

        static size_t GetNextAlignedAddress(size_t p_baseAddress, const size_t p_alignment)
        {
            return (p_baseAddress + (p_alignment - 1) & ~(p_alignment - 1));
        }

        static void* GetPreviousAlignedAddress(const void* p_baseAddress, const uint8_t p_alignment)
        {
            return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(p_baseAddress) - (reinterpret_cast<uintptr_t>(p_baseAddress) & (p_alignment - 1)));
        }
        static size_t GetPreviousAlignedAddress(size_t p_baseAddress, const size_t p_alignment)
        {
            return (p_baseAddress - (p_baseAddress & (p_alignment - 1)));
        }

        static constexpr size_t CalculatePadding(const size_t p_baseAddress, const uint8_t p_alignment)
        {
            const size_t padding = (((p_baseAddress / p_alignment) + 1) * p_alignment) - p_baseAddress;
            return (padding == p_alignment) ? 0 : padding;
        }
        static constexpr size_t CalculatePadding(const size_t p_baseAddress, const uint8_t p_alignment, size_t p_neededSize)
        {
            size_t padding = CalculatePadding(p_baseAddress, p_alignment);

            if (padding < p_neededSize)
            {
                p_neededSize -= padding;

                padding += p_alignment * (p_neededSize / p_alignment);
                if (p_neededSize % p_alignment > 0)
                    padding += p_alignment;
            }
            return padding;
        }

        

        uint64_t            m_allocationCount{ 0 };
        size_t              m_totalSize{ 0 };
        size_t              m_usedMemory{ 0 };
        size_t              m_peak{ 0 };
        u_memoryPtr<void>   m_basePtr{};


    };
}
