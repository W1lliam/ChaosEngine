#pragma once
#include "ChaosCore/chaos_core_pch.h"


namespace Chaos::Graphics::Buffers
{

#define CHAOS_FLOAT 0x1406
#define CHAOS_INT  0x1404
    class IVertexBuffer
    {
    public:
        virtual ~IVertexBuffer() = default;
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void Push(const unsigned count, const unsigned p_type) = 0;

        static IVertexBuffer*   Create(const void* p_data, const size_t p_size);
    };
}