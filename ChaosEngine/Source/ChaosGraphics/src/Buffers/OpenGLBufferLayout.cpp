#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Buffers/OpenGLBufferLayout.h"

namespace Chaos::Graphics::Buffers
{
    OpenGLBufferLayout::OpenGLBufferLayout(const OpenGLBufferLayout& p_other) : m_elements(p_other.m_elements), m_stride(p_other.m_stride)
	{
	}

	OpenGLBufferLayout::OpenGLBufferLayout(OpenGLBufferLayout&& other) noexcept : m_elements(std::move(other.m_elements)),
		m_stride(other.m_stride)
	{}

	OpenGLBufferLayout& OpenGLBufferLayout::operator=(const OpenGLBufferLayout& other)
	{
		if (this == &other)
			return *this;
		m_elements = other.m_elements;
		m_stride = other.m_stride;
		return *this;
	}

	OpenGLBufferLayout& OpenGLBufferLayout::operator=(OpenGLBufferLayout&& other) noexcept
	{
		if (this == &other)
			return *this;
		m_elements = std::move(other.m_elements);
		m_stride = other.m_stride;
		return *this;
	}

	void OpenGLBufferLayout::Push(const unsigned p_count, const unsigned p_type)
	{
		m_elements.push_back({ p_type, p_count, GL_FALSE });
		m_stride += p_count * OpenGLBufferElement::GetSizeOfType(p_type);
	}

	const std::vector<OpenGLBufferElement>& OpenGLBufferLayout::GetElements() const
	{
		return m_elements;
	}

	unsigned OpenGLBufferLayout::GetStride() const
	{
		return m_stride;
	}
}