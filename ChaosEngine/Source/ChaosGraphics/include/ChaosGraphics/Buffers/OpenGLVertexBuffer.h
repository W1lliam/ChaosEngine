#pragma once
#include "ChaosCore/Resources/Buffers/IVertexBuffer.h"
#include "ChaosGraphics/Buffers/OpenGLBufferLayout.h"

namespace Chaos::Graphics::Buffers
{
    class OpenGLVertexBuffer : public IVertexBuffer
    {
    public:
        OpenGLVertexBuffer(const void* p_data, const size_t p_size);
        ~OpenGLVertexBuffer() override;
        // Inherited via IVertexBuffer
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual void Push(const unsigned p_count, const unsigned p_type) override;

        inline OpenGLBufferLayout& GetLayout() { return m_layout; };

    private:
        uint32_t m_rendererID {0};
        OpenGLBufferLayout m_layout;
    };
}