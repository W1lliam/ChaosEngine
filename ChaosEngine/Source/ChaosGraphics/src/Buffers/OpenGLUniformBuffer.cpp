#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Buffers/OpenGLUniformBuffer.h"

namespace Chaos::Graphics::Buffers
{
	IUniformBuffer* IUniformBuffer::Create(uint8_t p_slot, const size_t p_size)
	{
		return new OpenGLUniformBuffer(p_slot, p_size);
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint8_t p_slot, const size_t p_size)
	{
		m_slot = p_slot;

		glGenBuffers(1, &m_rendererID);

		glBindBuffer(GL_UNIFORM_BUFFER, m_rendererID);
		glBufferData(GL_UNIFORM_BUFFER, p_size, NULL, GL_STATIC_DRAW);

		glBindBufferBase(GL_UNIFORM_BUFFER, m_slot, m_rendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLUniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_rendererID);
	}

	void OpenGLUniformBuffer::Bind(uint8_t p_slot)
	{
		m_slot = p_slot;

		glBindBufferBase(GL_UNIFORM_BUFFER, m_slot, m_rendererID);

		glBindBuffer(GL_UNIFORM_BUFFER, m_rendererID);
	}

	void OpenGLUniformBuffer::SetSlot(uint8_t p_slot)
	{
		m_slot = p_slot;
	}

	void OpenGLUniformBuffer::UpdateData(uint32_t p_padding, size_t p_dataSize, void* p_data) const
	{
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, p_padding, p_dataSize, p_data);
		Unbind();
	}
}