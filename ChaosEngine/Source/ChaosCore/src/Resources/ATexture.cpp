#include "ChaosCore/Resources/ATexture.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"
#include "ChaosCore/Resources/AssetLoader.h"

namespace Chaos::Resources
{
    ATexture::ATexture(AResourceManager* p_creator, const std::string& p_id, const StringID& p_group) :
        AResource(p_creator, operator""_sid(p_id.c_str(), p_id.size()), p_group)
    {}

    ATexture::~ATexture()
    {
        if (m_dataStream.is_open())
        {
            m_dataStream.close();
        }

        if (m_data)
            delete[] m_data;
    }

    void ATexture::PrepareImpl()
    {
        m_dataStream = ResourceGroupManager::Get().OpenResource(m_name, m_group);
        if (!AssetLoader::Validate(m_dataStream, AssetType::E_ASSETTYPE_TEXTURE))
        {
            throw std::exception((std::string("Asset " + m_name) + " is not valid").c_str());
        }
    }

    void ATexture::LoadImpl()
    {
        CHAOS_CORE_ASSERT(m_dataStream.is_open(), "Asset file has not been opened");

        //TODO: Either use name or remove from asset file
        std::string name;
        getline(m_dataStream, name, '\0');

        m_dataStream.read(reinterpret_cast<char*>(&m_width), sizeof(m_width));
        m_dataStream.read(reinterpret_cast<char*>(&m_height), sizeof(m_height));
        m_dataStream.read(reinterpret_cast<char*>(&m_bpp), sizeof(m_bpp));
        m_dataStream.read(reinterpret_cast<char*>(&m_internalTextureType), sizeof(E_TextureType));

        if (m_internalTextureType == E_TextureType::RGBA8)
        {
            m_data = new uint8_t[m_width * m_height * m_bpp];
            m_dataStream.read(reinterpret_cast<char*>(m_data), m_width * m_height * m_bpp);
        }
        else
        {
            m_data = new float[m_width * m_height * m_bpp];
            m_dataStream.read(reinterpret_cast<char*>(m_data), m_width * m_height * m_bpp * sizeof(float));
        }

    }

    void ATexture::UnprepareImpl()
    {
        if (m_dataStream.is_open())
        {
            m_dataStream.close();
        }
    }
}
