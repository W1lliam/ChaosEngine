#pragma once
#include "ChaosCore/Resources/Buffers/IIndexBuffer.h"

namespace Chaos::Graphics::Buffers
{
    class OpenGLIndexBuffer : public IIndexBuffer
    {
    public:
        OpenGLIndexBuffer(const uint32_t* p_indices, const uint32_t p_indicesCount);
        ~OpenGLIndexBuffer() override;
        // Inherited via IVertexBuffer
        virtual void        Bind() const override;
        virtual void        Unbind() const override;
        virtual uint32_t    Count() const override { return m_count; };

    private:
        uint32_t m_rendererID   {0};
        uint32_t m_count        {0};
    };
}