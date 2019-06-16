#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Buffers/OpenGLIndexBuffer.h"

namespace Chaos::Graphics::Buffers
{
    IIndexBuffer* IIndexBuffer::Create(const uint32_t* p_indices, const uint32_t p_indicesCount)
    {
        return new OpenGLIndexBuffer(p_indices, p_indicesCount);
    }

    OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* p_indices, const uint32_t p_indicesCount)
    {
        m_count = p_indicesCount;
        glGenBuffers(1, &m_rendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_indicesCount * sizeof(uint32_t), p_indices, GL_STATIC_DRAW);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        glDeleteBuffers(1, &m_rendererID);
    }

    void OpenGLIndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    }

    void OpenGLIndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}