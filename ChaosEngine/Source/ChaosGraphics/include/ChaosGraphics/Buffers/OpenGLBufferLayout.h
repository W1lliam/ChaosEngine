#pragma once

#pragma warning(push)
#pragma warning(disable: 4251)
#include "ChaosGraphics/chaos_graphics_pch.h"
#include <ChaosDebug.h>


namespace Chaos::Graphics::Buffers
{
	struct OpenGLBufferElement
	{
		unsigned int type;
		unsigned int count;
		unsigned char normalized;
		
		static unsigned int GetSizeOfType(const unsigned int p_type)
		{
			switch (p_type)
			{
			case GL_FLOAT:			return 4;
			case GL_INT:			return 4;
			case GL_UNSIGNED_INT:	return 4;
			case GL_UNSIGNED_BYTE:	return 1;
			default: CHAOS_CORE_ASSERT(false, "Unhandled type");
			}
            return 0;
		}
	};

	class OpenGLBufferLayout
	{
	private:
		std::vector<OpenGLBufferElement> m_elements{};
		unsigned int m_stride {0};

	public:
		OpenGLBufferLayout() = default;
		OpenGLBufferLayout(const OpenGLBufferLayout& p_other);
		OpenGLBufferLayout(OpenGLBufferLayout&& other) noexcept;
		OpenGLBufferLayout& operator=(const OpenGLBufferLayout& other);
		OpenGLBufferLayout& operator=(OpenGLBufferLayout&& other) noexcept;
		~OpenGLBufferLayout() = default;

		void Push(const unsigned p_count, const unsigned p_type);
		const std::vector<OpenGLBufferElement>& GetElements() const;
		unsigned int GetStride() const;
	};
}

#pragma warning(pop)