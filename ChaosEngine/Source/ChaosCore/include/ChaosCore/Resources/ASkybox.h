#pragma once
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"

namespace Chaos
{
    class SkyboxManager;
}

namespace Chaos::Resources
{

    struct SkyboxData
    {
        std::vector<float> environmentMap{};
        std::vector<float> irradianceMap{};
        std::vector<float> prefilterMap{};
    };

    class ASkybox : public AResource
    {
    friend class Chaos::SkyboxManager;
    public: 
        ASkybox(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group);
        virtual ~ASkybox();

        static  ASkybox*   Create(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group);
		static  ASkybox*   Create(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group, uint32_t& p_width, uint32_t& p_height);

        static SkyboxData           PrecomputeSkybox(float* p_data, uint32_t p_width, uint32_t p_height, uint8_t p_bpp, uint32_t p_environmentMapSize = 1024u, uint32_t p_prefilteredMapSize = 512);
        static std::vector<float>   GenerateBRDF();


        virtual void        Bind                (const uint32_t p_slot = 0) const   = 0;
        virtual void        BindIrradiance      (const uint32_t p_slot = 0) const   = 0;
        virtual void        BindPrefilter       (const uint32_t p_slot = 0) const   = 0;
        virtual void        Unbind()                                        const   = 0;

        virtual void        Render()                                                = 0;

		virtual uint32_t&	GetTextureID()							                = 0;

    protected:
        virtual void PrepareImpl()      override;
        virtual void LoadImpl()         override;

        virtual void UnprepareImpl()    override;

    protected:
        static constexpr    uint32_t                        MAX_TEXTURE_COUNT{ 256 };
        static              Chaos::Memory::PoolAllocator    s_allocator;


        std::ifstream   m_dataStream{};
        SkyboxData      m_skyboxData;
		uint32_t		m_environmentSize;
		uint32_t		m_irradianceSize;
		uint32_t		m_prefilteredSize;
    };

    using SkyboxPtr = std::shared_ptr<ASkybox>;
}