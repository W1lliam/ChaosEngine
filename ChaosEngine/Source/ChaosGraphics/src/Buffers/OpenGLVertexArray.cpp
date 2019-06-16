#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Buffers/OpenGLBufferLayout.h"
#include "ChaosGraphics/Buffers/OpenGLVertexArray.h"
#include "ChaosGraphics/Buffers/OpenGLVertexBuffer.h"


namespace Chaos::Graphics::Buffers
{
    IVertexArray* IVertexArray::Create()
    {
        return new OpenGLVertexArray;
    }

    OpenGLVertexArray:: OpenGLVertexArray()
    {
        glGenVertexArrays(1, &m_rendererID);
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_rendererID);
    }

    void OpenGLVertexArray::AddBuffer(const IVertexBuffer& p_vertexBuffer) const
    {
        Bind();
        p_vertexBuffer.Bind();
        OpenGLVertexBuffer& glVertexBuffer = (OpenGLVertexBuffer&)p_vertexBuffer;
        std::vector<OpenGLBufferElement> elements = glVertexBuffer.GetLayout().GetElements();
        unsigned int offset = 0;
        for (unsigned int i = 0; i < elements.size(); ++i)
        {
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, elements[i].count, elements[i].type, elements[i].normalized, glVertexBuffer.GetLayout().GetStride(), reinterpret_cast<const void*>(static_cast<const intptr_t>(offset)));
            offset += elements[i].count * OpenGLBufferElement::GetSizeOfType(elements[i].type);
        }
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_rendererID);
    }

    void OpenGLVertexArray::Unbind()
    {
        glBindVertexArray(0);
    }


}