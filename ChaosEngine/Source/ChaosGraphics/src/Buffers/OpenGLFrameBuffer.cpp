#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Buffers/OpenGLFrameBuffer.h"
#include "ChaosCore/Context/IWindow.h"
#include "ChaosCore/Rendering/IRenderer.h"

namespace Chaos::Graphics::Buffers
{
	IFrameBuffer* IFrameBuffer::Create(Chaos::Resources::TexturePtr p_texture)
	{
		return new OpenGLFrameBuffer(p_texture);
	}
	IFrameBuffer* IFrameBuffer::Create(std::vector<Chaos::Resources::TexturePtr> p_textures)
	{
		return new OpenGLFrameBuffer(p_textures);
	}

    void IFrameBuffer::Bind(uint32_t p_frameBufferID)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, p_frameBufferID);
    }

	OpenGLFrameBuffer::OpenGLFrameBuffer(Chaos::Resources::TexturePtr p_texture)
	{
		RegisterEventCallback<Events::WindowResizeEvent>(std::bind(&OpenGLFrameBuffer::OnWindowResize, this, std::placeholders::_1));

		m_texture		= p_texture;

		glGenFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		if (p_texture->Type() != GL_DEPTH_COMPONENT)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetTextureID(), 0);

			glGenTextures(1, &m_depthID);
			glBindTexture(GL_TEXTURE_2D, m_depthID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_texture->Width(), m_texture->Height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthID, 0);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture->GetTextureID(), 0);
		}

		if (!CheckCompleteStatus())
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}

		Unbind();
	}

	void OpenGLFrameBuffer::SetTexture(Chaos::Resources::TexturePtr p_texture)
	{
		m_texture = p_texture;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetTextureID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		if (!CheckCompleteStatus())
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(std::vector<Chaos::Resources::TexturePtr> p_textures)
	{
		RegisterEventCallback<Events::WindowResizeEvent>(std::bind(&OpenGLFrameBuffer::OnWindowResize, this, std::placeholders::_1));

		m_textures	= p_textures;
		m_texture	= p_textures[0];

		glGenFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		bool hasDepth = false;
		std::vector<GLenum> attachments;
		for (int i = 0; i < p_textures.size(); ++i)
		{
			if (p_textures[i]->Type() == GL_DEPTH_COMPONENT && !hasDepth)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, p_textures[i]->GetTextureID(), 0);
				hasDepth = true;
			}
			else
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, p_textures[i]->GetTextureID(), 0);
				
				if(!hasDepth)
					attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
				else
					attachments.push_back(GL_COLOR_ATTACHMENT0 + (i - 1));

			}
		}
		if (!hasDepth)
		{
			glGenTextures(1, &m_depthID);
			glBindTexture(GL_TEXTURE_2D, m_depthID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_texture->Width(), m_texture->Height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthID, 0);

			glBindTexture(GL_TEXTURE_2D, 0);

		}
		else if(1 >= p_textures.size())
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		glDrawBuffers((GLsizei)attachments.size(), &attachments[0]);

		if (!CheckCompleteStatus())
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}

		Unbind();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		if(m_textures.size() > 0)	m_textures.clear();
		glDeleteFramebuffers(1, &m_rendererID);
	}

	bool OpenGLFrameBuffer::CheckCompleteStatus() const
	{
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}

	Resources::TexturePtr OpenGLFrameBuffer::GetTexture()
	{
		return m_texture;
	}

	std::vector<Chaos::Resources::TexturePtr> OpenGLFrameBuffer::GetTextures()
	{
		return m_textures;
	}

	bool OpenGLFrameBuffer::OnWindowResize(const Events::WindowResizeEvent & p_event)
	{
		if (m_textures.size() > 0)
		{
			for (int i = 0; i < m_textures.size(); ++i)
			{
				m_textures[i]->SetSize(p_event.GetWidth(), p_event.GetHeight());
			}
		}

		m_texture->SetSize(p_event.GetWidth(), p_event.GetHeight());
		
		if (m_rboID != 0)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_rboID);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_texture->Width(), m_texture->Height());
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		if (m_depthID != 0)
		{
			glBindTexture(GL_TEXTURE_2D, m_depthID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_texture->Width(), m_texture->Height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		return false;
	}

	void OpenGLFrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		auto& renderer = IRenderer::Get();
		glViewport(renderer.GetViewportXOffset(), renderer.GetViewportYOffset(), IRenderer::Get().ViewportWidth, IRenderer::Get().ViewportHeight);
	}

	void OpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);


		if (m_texture->Type() != GL_DEPTH_COMPONENT)
		{
			auto& renderer = IRenderer::Get();
			glViewport(0, 0, renderer.GetWidth(), renderer.GetHeight());
		}
		else
		{
			glViewport(0, 0, m_texture->Width(), m_texture->Height());
		}
	}
	void OpenGLFrameBuffer::BindForDrawing() const
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_rendererID);
	}
	void OpenGLFrameBuffer::BindForReading() const
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
	}
	void OpenGLFrameBuffer::BindTexture() const
	{
		m_texture->Bind();
	}
	void OpenGLFrameBuffer::BindTextures() const
	{
		if (0 < m_textures.size())
		{
			for (uint8_t i = 0; i < m_textures.size(); ++i)
			{
				m_textures[i]->Bind(i);
			}
		}
	}
}