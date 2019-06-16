#pragma once
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"

namespace Chaos
{
    class TextureManager;
	enum E_TextureType : uint32_t;
}

namespace Chaos::Resources
{
    class ATexture : public AResource
    {
    friend class Chaos::TextureManager;
    public: 
        ATexture(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group);
        virtual ~ATexture();

        static  ATexture*   Create(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group);
		static  ATexture*   Create(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group, uint32_t& p_width, uint32_t& p_height, E_TextureType p_type);

        virtual void        Bind   (const uint32_t p_slot = 0) const = 0;
        virtual void        Unbind ()                          const = 0;

		virtual uint32_t&	GetTextureID()							 = 0;

		virtual void		SetSize(uint32_t width, uint32_t height) = 0;

                uint32_t    Width()				const { return m_width;	};
                uint32_t    Height()			const { return m_height; };
                uint32_t    BPP()				const { return m_bpp;	};
				uint32_t	Type()				const { return m_textureType; };
				uint32_t	TypeInternal()		const { return m_internalTextureType; };

    protected:
        virtual void PrepareImpl()      override;
        virtual void LoadImpl()         override;

        virtual void UnprepareImpl()    override;

    protected:
        static constexpr    uint32_t                        MAX_TEXTURE_COUNT{ 256 };
        static              Chaos::Memory::PoolAllocator    s_allocator;


        std::ifstream   m_dataStream{};
        void*        m_data      {nullptr};
        uint32_t        m_width;
        uint32_t        m_height;
		uint32_t		m_textureType;
        uint32_t	    m_formatType    {0};
        E_TextureType	m_internalTextureType;
        uint8_t	        m_bpp           {0};

    };

    using TexturePtr = std::shared_ptr<ATexture>;
}