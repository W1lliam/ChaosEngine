#pragma once
#include "ChaosCore/Resources/ATexture.h"

namespace Chaos::Resources
{
    class OpenGLTexture : public ATexture
    {
    public:
        OpenGLTexture(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group);
        OpenGLTexture(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group, uint32_t& p_width, uint32_t& p_height, E_TextureType p_type);
        virtual ~OpenGLTexture() = default;

        virtual void PostLoadImpl()     override;
        virtual void UnloadImpl()       override;

		virtual uint32_t& GetTextureID() override;

		virtual void	  SetSize(uint32_t width, uint32_t height) override;

        virtual void Bind(const uint32_t p_slot = 0)    const   override;
        virtual void Unbind()                           const   override;
    protected:
        uint32_t m_rendererID;
    };
}