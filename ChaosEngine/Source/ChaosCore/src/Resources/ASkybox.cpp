#include "ChaosCore/Resources/ASkybox.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"
#include "ChaosCore/Resources/AssetLoader.h"

namespace Chaos::Resources
{
    ASkybox::ASkybox(AResourceManager* p_creator, const std::string& p_id, const StringID& p_group) :
        AResource(p_creator, operator""_sid(p_id.c_str(), p_id.size()), p_group)
    {}

    ASkybox::~ASkybox()
    {
        if (m_dataStream.is_open())
        {
            m_dataStream.close();
        }
    }

    void ASkybox::PrepareImpl()
    {
        m_dataStream = ResourceGroupManager::Get().OpenResource(m_name, m_group);
        if (!AssetLoader::Validate(m_dataStream, AssetType::E_ASSETTYPE_SKYBOX))
        {
            throw std::exception((std::string("Asset " + m_name) + " is not valid").c_str());
        }
    }

    void ASkybox::LoadImpl()
    {
        CHAOS_CORE_ASSERT(m_dataStream.is_open(), "Asset file has not been opened");

        std::string name;
        getline(m_dataStream, name, '\0');

        m_dataStream.read(reinterpret_cast<char*>(&m_environmentSize),      sizeof(m_environmentSize));
        m_dataStream.read(reinterpret_cast<char*>(&m_irradianceSize),       sizeof(m_irradianceSize));
        m_dataStream.read(reinterpret_cast<char*>(&m_prefilteredSize),      sizeof(m_prefilteredSize));

        m_skyboxData.environmentMap.resize(m_environmentSize);
        m_skyboxData.irradianceMap.resize(m_irradianceSize);
        m_skyboxData.prefilterMap.resize(m_prefilteredSize);


        //Environment
        const   uint8_t     envMipMapCount  = static_cast<uint8_t>(1 + floor(std::log2(m_environmentSize)));
                uint32_t    floatsWritten   = 0;

        for (uint8_t mip = 0; mip < envMipMapCount; mip++)
        {
            unsigned int mipSize = static_cast<unsigned int>(m_environmentSize * std::pow(0.5, mip));
            for (unsigned int i = 0; i < 6; ++i)
            {
                floatsWritten += mipSize * mipSize * 3;
            }
        }
        m_skyboxData.environmentMap.resize(floatsWritten);
        m_dataStream.read(reinterpret_cast<char*>(&m_skyboxData.environmentMap[0]), floatsWritten * sizeof(float));

        //Irradiance
        m_skyboxData.irradianceMap.resize(m_irradianceSize * m_irradianceSize * 3 * 6);
        m_dataStream.read(reinterpret_cast<char*>(&m_skyboxData.irradianceMap[0]), m_irradianceSize * m_irradianceSize * 3 * 6 * sizeof(float));

        //Prefiltered
        const uint8_t preFilterMipMapCount = static_cast<uint8_t>(1 + floor(std::log2(m_prefilteredSize)));
        floatsWritten = 0;
        for (uint8_t mip = 0; mip < preFilterMipMapCount; mip++)
        {
            unsigned int mipSize = static_cast<unsigned int>(m_prefilteredSize * std::pow(0.5, mip));
            for (unsigned int i = 0; i < 6; ++i)
            {
                floatsWritten += mipSize * mipSize * 3;
            }
        }
        m_skyboxData.prefilterMap.resize(floatsWritten);
        m_dataStream.read(reinterpret_cast<char*>(&m_skyboxData.prefilterMap[0]), floatsWritten * sizeof(float));
    }

    void ASkybox::UnprepareImpl()
    {
        if (m_dataStream.is_open())
        {
            m_dataStream.close();
        }
    }
}
