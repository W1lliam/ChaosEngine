#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Resources/OpenGLTexture.h"
#include "ChaosCore/Resources/Managers/TextureManager.h"

namespace Chaos::Resources
{
    Memory::PoolAllocator ATexture::s_allocator { sizeof(Resources::OpenGLTexture) * MAX_TEXTURE_COUNT, sizeof(Resources::OpenGLTexture), alignof(Resources::OpenGLTexture) };

    ATexture* ATexture::Create(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group)
    {
        return new (s_allocator.Allocate(sizeof(Resources::OpenGLTexture), alignof(Resources::OpenGLTexture))) OpenGLTexture(p_creator, p_id, p_group);
    }

	ATexture* ATexture::Create(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group, uint32_t& p_width, uint32_t& p_height, E_TextureType p_type)
	{
		return new (s_allocator.Allocate(sizeof(Resources::OpenGLTexture), alignof(Resources::OpenGLTexture))) OpenGLTexture(p_creator, p_id, p_group, p_width, p_height, p_type);
	}

    OpenGLTexture::OpenGLTexture(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group) : 
        ATexture(p_creator, p_id, p_group)
    {
    }

	OpenGLTexture::OpenGLTexture(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group, uint32_t& p_width, uint32_t& p_height, E_TextureType p_type) : 
        ATexture(p_creator, p_id, p_group)
	{
		m_width					= p_width;
		m_height				= p_height;
		m_internalTextureType	= p_type;

		switch (m_internalTextureType)
		{
		case RGBA8:
			m_textureType	= GL_RGBA;
			m_formatType	= GL_UNSIGNED_BYTE;
			break;
		case RGB:
		case RG32F:
		case RGB16F:
		case RGB32F:
			m_textureType	= GL_RGB;
			m_formatType	= GL_FLOAT;
			break;
		case DEPTH:
		case DEPTH16:
		case DEPTH24:
			m_textureType	= GL_DEPTH_COMPONENT;
			m_formatType	= GL_FLOAT;
			break;
		}

		PostLoadImpl();
	}

    void OpenGLTexture::PostLoadImpl()
    {
        glGenTextures(1, &m_rendererID);
		if (m_data) 
            glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_rendererID);


		if (m_data)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            switch (m_internalTextureType)
            {
            case RGBA8:
                m_textureType   = GL_RGBA;
                m_formatType    = GL_UNSIGNED_BYTE;
                break;
            case RG16F:
            case RG32F:
                m_textureType   = GL_RG;
                m_formatType    = GL_FLOAT;
                break;
            case RGB:
            case RGB16F:
            case RGB32F:
                m_textureType   = GL_RGB;
                m_formatType    = GL_FLOAT;
                break;
            case DEPTH:
            case DEPTH16:
            case DEPTH24:
                m_textureType   = GL_DEPTH_COMPONENT;
                m_formatType    = GL_FLOAT;
                break;
            }
			
            glTexImage2D(GL_TEXTURE_2D, 0, (uint32_t)m_internalTextureType, m_width, m_height, 0, m_textureType, m_formatType, m_data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glGenerateMipmap(GL_TEXTURE_2D);


		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, m_internalTextureType, m_width, m_height, 0, m_textureType, m_formatType, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if(m_textureType == GL_DEPTH_COMPONENT)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

				float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			}
		}





        glBindTexture(GL_TEXTURE_2D, 0);
		m_state.store(E_ResourceState::LOADED);
    }

    void OpenGLTexture::UnloadImpl()
    {
        glDeleteTextures(1, &m_rendererID);
        delete[] m_data;
    }

	uint32_t& OpenGLTexture::GetTextureID()
	{
		return m_rendererID;
	}

    void OpenGLTexture::Bind(const uint32_t p_slot) const
    {
        glActiveTexture(GL_TEXTURE0 + p_slot);
        glBindTexture(GL_TEXTURE_2D, m_rendererID);
    }

    void OpenGLTexture::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

	void OpenGLTexture::SetSize(uint32_t p_width, uint32_t p_height)
	{
		m_width		= p_width;
		m_height	= p_height;

		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, m_internalTextureType, m_width, m_height, 0, m_textureType, GL_UNSIGNED_BYTE, NULL);
		Unbind();
	}
}