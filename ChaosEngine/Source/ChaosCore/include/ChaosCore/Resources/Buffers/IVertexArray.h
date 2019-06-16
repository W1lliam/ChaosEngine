#pragma once
#include "ChaosCore/chaos_core_pch.h"


namespace Chaos::Graphics::Buffers
{
    class IVertexBuffer;
    class OpenGLBufferLayout;

    class IVertexArray
    {

    public:
        virtual ~IVertexArray() = default;

        virtual void AddBuffer(const IVertexBuffer& p_vertexBuffer) const = 0;

        virtual void Bind() const = 0;
        virtual void Unbind() = 0;

        static  IVertexArray*   Create();

    };
}