#pragma once
#include "ChaosCore/Resources/Buffers/IVertexArray.h"

namespace Chaos::Graphics::Buffers
{
    class OpenGLVertexArray : public IVertexArray
    {
    public:
        OpenGLVertexArray();
        ~OpenGLVertexArray();
        virtual void AddBuffer(const IVertexBuffer & p_vertexBuffer) const override;
        virtual void Bind() const override;
        virtual void Unbind() override;



    private:
        unsigned int m_rendererID;
    };
}