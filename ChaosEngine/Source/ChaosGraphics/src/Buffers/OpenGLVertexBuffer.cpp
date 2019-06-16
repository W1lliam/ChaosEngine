#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Buffers/OpenGLVertexBuffer.h"


namespace Chaos::Graphics::Buffers
{
    IVertexBuffer* IVertexBuffer::Create(const void* p_data, const size_t p_size)
    {
        return new OpenGLVertexBuffer(p_data, p_size);
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(const void* p_data, const size_t p_size)
    {
        glGenBuffers(1, &m_rendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
        glBufferData(GL_ARRAY_BUFFER, p_size, p_data, GL_STATIC_DRAW);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        glDeleteBuffers(1, &m_rendererID);
    }

    void OpenGLVertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    }

    void OpenGLVertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void OpenGLVertexBuffer::Push(const unsigned p_count, const unsigned p_type)
    { 
        m_layout.Push(p_count, p_type);
    }
}