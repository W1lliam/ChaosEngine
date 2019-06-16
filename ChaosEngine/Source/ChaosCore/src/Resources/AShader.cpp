#include "ChaosCore/Resources/AShader.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"

namespace Chaos::Resources
{
    AShader::AShader(AResourceManager * p_creator, const StringID & p_name, const StringID & p_group) : AResource(p_creator, p_name, p_group)
    {}

    void AShader::PrepareImpl()
    {
        std::ifstream& shaderFile = ResourceGroupManager::Get().OpenResource(m_name, m_group);
        if (!shaderFile.is_open())
        {
            CHAOS_CORE_WARN("Error reading shader {0}", m_name);
            shaderFile.close();
            return;
        }

        std::string shader;
        shaderFile.seekg(0, std::ios::end);
        shader.reserve(shaderFile.tellg());
        shaderFile.seekg(0, std::ios::beg);

        shader.assign(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
        shaderFile.close();

        const size_t vtStartIndex       = shader.find(VERTEX_DELIMITER) + VERTEX_DELIMITER.size();
        const size_t fragStartIndex     = shader.find(FRAGMENT_DELIMITER);
        const size_t geometryStartIndex = shader.find(GEOMETRY_DELIMITER);

        if (shader.size() != 0 && vtStartIndex != 0 && fragStartIndex != 0)
        {
            if (geometryStartIndex == std::string::npos)
            {
                m_sources.VertexSource      = shader.substr(vtStartIndex, fragStartIndex - vtStartIndex);
                m_sources.FragmentSource    = shader.substr(fragStartIndex + FRAGMENT_DELIMITER.size());
            }
            else
            {
                m_sources.VertexSource      = shader.substr(vtStartIndex, geometryStartIndex - vtStartIndex);
                m_sources.GeometrySource    = shader.substr(geometryStartIndex + GEOMETRY_DELIMITER.size(), fragStartIndex - geometryStartIndex - GEOMETRY_DELIMITER.size());
                m_sources.FragmentSource    = shader.substr(fragStartIndex + FRAGMENT_DELIMITER.size());
            }
        }
        else
        {
            CHAOS_CORE_WARN("Error finding shaders for {0}", m_name);
        }
    }
    void AShader::UnprepareImpl()
    {
        m_sources.VertexSource.clear();
        m_sources.GeometrySource.clear();
        m_sources.FragmentSource.clear();
    }
}