#pragma once
#include "ChaosCore/chaos_core_pch.h"


namespace Chaos::Graphics::Buffers
{
    class IIndexBuffer
    {
        public :
            virtual ~IIndexBuffer() = default;
            virtual void        Bind() const = 0;
            virtual void        Unbind() const = 0;
            virtual uint32_t    Count() const = 0;

            static  IIndexBuffer*   Create(const uint32_t* p_indices, const uint32_t p_indicesCount);

    };
}