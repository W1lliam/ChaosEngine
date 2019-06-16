#include "ChaosCore/Resources/Material.h"
#include <string>
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"
#include "ChaosCore/Resources/Managers/TextureManager.h"
#include "ChaosCore/Resources/AssetLoader.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Systems/SceneSystem.h"

namespace Chaos
{

	Resources::Material::Material(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group, ShaderPtr p_shader) :
		AResource(p_creator, operator""_sid(p_name.c_str(), p_name.size()), p_group)
	{
        if(p_shader)
		    SetShader(p_shader);
	}

	void Resources::Material::SetShader(Resources::ShaderPtr p_shader)
	{
        m_textures.clear();
        m_uniforms.clear();
		uint32_t samplerSlot = 0;
		m_shader = p_shader;
        if (p_shader)
        {
            for (const Uniform& shaderUniform : m_shader->GetUniforms())
            {
                if (shaderUniform.location != -1)
                {
                    if (IsSamplerType(shaderUniform.type))
                        m_textures[shaderUniform.name] = { shaderUniform.type, samplerSlot++ };
                    else
                        m_uniforms[shaderUniform.name] = { shaderUniform.type };
                }
            }
        }
		m_state.store(E_ResourceState::LOADED);
	}

	void Resources::Material::SetTexture(std::string p_name, Resources::TexturePtr p_texture)
	{
        m_textures[p_name].type = E_ShaderType::SHADER_TYPE_SAMPLER2D;
		m_textures[p_name].texture = p_texture;
	}
    void Resources::Material::SetTexture(std::string p_name, Resources::SkyboxPtr p_texture, E_ShaderType p_mapType)
    {
        m_textures[p_name].type = p_mapType;
        m_textures[p_name].cubeMap = p_texture;
    }

	void Resources::Material::SetUniform(std::string name, Maths::Matrix4& p_value)
	{
        if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_MAT4)
        {
            if ((int64_t)m_uniforms[name].value.asVoidPtr != -1)
                delete m_uniforms[name].value.asVoidPtr;

            m_uniforms[name].value.asVoidPtr = new Maths::Matrix4(p_value);
        }
	}

	void Resources::Material::SetUniform(std::string name, Maths::Quaternion & p_value)
	{
		if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_VEC4)
		{
			if ((int64_t)m_uniforms[name].value.asVoidPtr != -1)
				delete m_uniforms[name].value.asVoidPtr;

			m_uniforms[name].value.asVoidPtr = new Maths::Vector4(p_value.GetX(), p_value.GetY(), p_value.GetZ(), p_value.GetRealValue());
		}
	}

    void Resources::Material::SetUniform(std::string name, Maths::Matrix3 & p_value)
    {
        if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_MAT3)
        {
            if ((int64_t)m_uniforms[name].value.asVoidPtr != -1)
                delete m_uniforms[name].value.asVoidPtr;

            m_uniforms[name].value.asVoidPtr = new Maths::Matrix3(p_value);
        }
    }

    void Resources::Material::SetUniform(std::string name, Maths::Vector4 & p_value)
    {
        if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_VEC4)
        {
            if ((int64_t)m_uniforms[name].value.asVoidPtr != -1)
                delete m_uniforms[name].value.asVoidPtr;

            m_uniforms[name].value.asVoidPtr = new Maths::Vector4(p_value);
            m_uniforms[name].elementCount = 1;
        }
    }

	void Resources::Material::SetUniform(std::string name, Maths::Vector3 & p_value)
	{
		if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_VEC3)
		{
			if ((int64_t)m_uniforms[name].value.asVoidPtr != -1)
				delete m_uniforms[name].value.asVoidPtr;

			m_uniforms[name].value.asVoidPtr = new Maths::Vector3(p_value);
			m_uniforms[name].elementCount = 1;
		}
	}

	void Resources::Material::SetUniform(std::string name, Maths::Vector2 & p_value)
	{
		if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_VEC2)
		{
			if ((int64_t)m_uniforms[name].value.asVoidPtr != -1)
				delete m_uniforms[name].value.asVoidPtr;

			m_uniforms[name].value.asVoidPtr = new Maths::Vector2(p_value);
		}
	}

	void Resources::Material::SetUniform(std::string name, float p_value)
	{
		if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_FLOAT)
		{
			m_uniforms[name].value.asUIntPtr = 0;
			*reinterpret_cast<float*>(&m_uniforms[name].value.asVoidPtr) = p_value;
		}
	}

	void Resources::Material::SetUniform(std::string name, uint32_t p_value)
	{
		if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_UNSIGNED_INT)
		{
			m_uniforms[name].value.asUIntPtr = 0;
			*reinterpret_cast<uint32_t*>(&m_uniforms[name].value.asVoidPtr) = p_value;
		}
	}

	void Resources::Material::SetUniform(std::string name, int32_t p_value)
	{
		if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_INT)
		{
			m_uniforms[name].value.asUIntPtr = 0;
			*reinterpret_cast<int32_t*>(&m_uniforms[name].value.asVoidPtr) = p_value;
		}
	}

	void Resources::Material::SetUniform(std::string name, bool p_value)
	{
		if (m_uniforms[name].type == E_ShaderType::SHADER_TYPE_BOOL)
		{
			m_uniforms[name].value.asUIntPtr = 0;
			*reinterpret_cast<bool*>(&m_uniforms[name].value.asVoidPtr) = p_value;
		}
	}

	void Resources::Material::SetUniform(const std::string p_name, const std::vector<Maths::Vector3>& p_value)
	{
		if (m_uniforms[p_name].type == E_ShaderType::SHADER_TYPE_VEC4)
		{
			if ((int64_t)m_uniforms[p_name].value.asVoidPtr != -1)
				delete m_uniforms[p_name].value.asVoidPtr;

			m_uniforms[p_name].value.asVoidPtr = new std::vector<Maths::Vector3>(p_value);
			m_uniforms[p_name].elementCount = static_cast<uint32_t>(p_value.size());
		}
	}

	void Resources::Material::SetUniform(const std::string p_name, const std::vector<Maths::Vector4>& p_value)
	{
		if (m_uniforms[p_name].type == E_ShaderType::SHADER_TYPE_VEC4)
		{
			if ((int64_t)m_uniforms[p_name].value.asVoidPtr != -1)
				delete m_uniforms[p_name].value.asVoidPtr;

			m_uniforms[p_name].value.asVoidPtr = new std::vector<Maths::Vector4>(p_value);
			m_uniforms[p_name].elementCount = static_cast<uint32_t>(p_value.size());
		}
	}
	void Resources::Material::Bind() const
	{
		m_shader->Bind();
		for (const Uniform& shaderUniform : m_shader->GetUniforms())
		{
			auto uniformFoundIt = m_uniforms.find(shaderUniform.name);
			if (shaderUniform.location != -1 && uniformFoundIt != m_uniforms.end() && uniformFoundIt->second.value.asUIntPtr != (int)(-1))
			{
				    m_shader->SetUniform(std::move(shaderUniform.name), uniformFoundIt->second.value.asVoidPtr, uniformFoundIt->second.type, uniformFoundIt->second.elementCount);
			}
            else
            {
                auto textureFoundIt = m_textures.find(shaderUniform.name);
                if (shaderUniform.location != -1 && textureFoundIt != m_textures.end() && textureFoundIt->second.slot != (int)(-1))
                {
                    switch (m_textures.at(shaderUniform.name).type)
                    {
                        case E_ShaderType::SHADER_TYPE_SAMPLERCUBE_IRR :
                            m_textures.at(shaderUniform.name).cubeMap->BindIrradiance(m_textures.at(shaderUniform.name).slot);
                            break;
                        case E_ShaderType::SHADER_TYPE_SAMPLERCUBE_PREFILTER :
                            m_textures.at(shaderUniform.name).cubeMap->BindPrefilter(m_textures.at(shaderUniform.name).slot);
                            break;
                    default:
                        if (m_textures.at(shaderUniform.name).texture)
                            m_textures.at(shaderUniform.name).texture->Bind(m_textures.at(shaderUniform.name).slot);
                        else
                            TextureManager::Get().GetTexture("MissingTexture"_sid, "DefaultResources"_sid)->Bind(m_textures.at(shaderUniform.name).slot);
                        break;
                    }

                    m_shader->SetUniform(std::move(shaderUniform.name), m_textures.at(shaderUniform.name).slot);
                }
            }
		}
	}

    void Resources::Material::Rename(const std::string & p_newName)
    {
        std::string oldName(m_name.string);
        if (p_newName == oldName)
            return;

        std::string currentSceneName = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene()->GetFullName();
        if (filesystem::exists("Assets/" + currentSceneName + ".resdecl"))
        {
            std::ifstream readFile("Assets/" + currentSceneName + ".resdecl", std::ios::in);
            if (readFile.is_open())
            {
                std::string content(static_cast<std::stringstream const&>(std::stringstream() << readFile.rdbuf()).str());

                //Search & Replace
                auto pos = content.find(oldName);
                while (pos != std::string::npos) {
                    content.replace(pos, oldName.length(), p_newName);
                    pos = content.find(oldName, pos);
                }

                readFile.close();
                std::ofstream writeFile("Assets/" + currentSceneName + ".resdecl", std::ios::out | std::ios::trunc);
                if (writeFile.is_open())
                {
                    writeFile << content;
                }
                writeFile.close();
            }
            else
            {
                readFile.close();
            }
        }
        AResource::Rename(p_newName);
    }

    void Resources::Material::Save()
    {
        std::string filepath = ResourceGroupManager::Get().GetResourcePath(m_name, m_group);
        if (filepath.empty())
        {
            filepath = "Assets/Materials/";
            if (!std::filesystem::is_directory(filepath))
                std::filesystem::create_directory(filepath);
            filepath += std::string(m_name.string) + CHAOS_ASSET_EXTENSION;
        }

        std::ofstream dataStream(filepath, std::ios::out | std::ios::binary);

        if (dataStream.is_open())
        {
            ResourceHeader header(E_ASSETTYPE_MATERIAL);
            WriteStream writeStream(1024);

            header.Serialize(writeStream);

            SerializeStringID(writeStream, const_cast<StringID&>(m_shader->GetName()));
            SerializeStringID(writeStream, const_cast<StringID&>(m_shader->GetGroup()));
            

            uint32_t uniformsCount = static_cast<uint32_t>(m_uniforms.size());
            SerializeUnsignedInt(writeStream, uniformsCount, 0, UINT32_MAX);

            for (std::pair<std::string, UniformValue> p_pair : m_uniforms)
            {
                bool isUniformSet = (int64_t)p_pair.second.value.asVoidPtr != -1;
                SerializeBool(writeStream, isUniformSet);

                if (isUniformSet)
                {
                    SerializeString(writeStream, p_pair.first);
                    uint16_t uniformType = static_cast<uint16_t>(p_pair.second.type);
                    SerializeUnsignedInt(writeStream, uniformType, 0, static_cast<uint16_t>(E_ShaderType::SHADER_TYPE_MAX));

                    switch (p_pair.second.type)
                    {
                        case E_ShaderType::SHADER_TYPE_BOOL:
                        {
                            SerializeBool(writeStream, *reinterpret_cast<bool*>(&p_pair.second.value.asVoidPtr));
                            break;
                        }
                        case E_ShaderType::SHADER_TYPE_INT:
                        {
                            SerializeSignedInt(writeStream, *reinterpret_cast<int32_t*>(&p_pair.second.value.asVoidPtr), 0, INT32_MAX);
                            break;
                        }
                        case E_ShaderType::SHADER_TYPE_UNSIGNED_INT:
                        {
                            SerializeUnsignedInt(writeStream, *reinterpret_cast<uint32_t*>(&p_pair.second.value.asVoidPtr),0 ,UINT32_MAX);
                            break;
                        }
                        case E_ShaderType::SHADER_TYPE_FLOAT:
                        {
                            SerializeFloat(writeStream, *reinterpret_cast<float*>(&p_pair.second.value.asVoidPtr));
                            break;
                        }
                        case E_ShaderType::SHADER_TYPE_VEC2:
                        {
                            Maths::Vector2* value = (Maths::Vector2*)(p_pair.second.value.asVoidPtr);
                            SerializeFloat(writeStream, value->x);
                            SerializeFloat(writeStream, value->y);
                            break;
                        }
                        case E_ShaderType::SHADER_TYPE_VEC3:
                        {
                            Maths::Vector3* value = (Maths::Vector3*)(p_pair.second.value.asVoidPtr);
                            SerializeFloat(writeStream, value->x);
                            SerializeFloat(writeStream, value->y);
                            SerializeFloat(writeStream, value->z);
                            break;
                        }
                        case E_ShaderType::SHADER_TYPE_VEC4:
                        {
                            Maths::Vector4* value = (Maths::Vector4*)(p_pair.second.value.asVoidPtr);
                            SerializeFloat(writeStream, value->x);
                            SerializeFloat(writeStream, value->y);
                            SerializeFloat(writeStream, value->z);
                            SerializeFloat(writeStream, value->w);
                            break;
                        }
                        case E_ShaderType::SHADER_TYPE_MAT3:
                        {
                            Maths::Matrix3* value = (Maths::Matrix3*)(p_pair.second.value.asVoidPtr);
                            for (int i = 0; i < 9; ++i)
                            {
                                SerializeFloat(writeStream, value->elements[i]);
                            }
                            break;
                        }
                        case E_ShaderType::SHADER_TYPE_MAT4:
                        {
                            Maths::Matrix4* value = (Maths::Matrix4*)(p_pair.second.value.asVoidPtr);
                            for (int i = 0; i < 16; ++i)
                            {
                                SerializeFloat(writeStream, value->elements[i]);
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
                
            }

            uint32_t textureUniformsCount = static_cast<uint32_t>(m_textures.size());
            SerializeUnsignedInt(writeStream, textureUniformsCount, 0, UINT32_MAX);

            for (std::pair<std::string, UniformValueSampler> p_pair : m_textures)
            {
                bool isTextureBound = p_pair.second.texture != nullptr;
                SerializeBool(writeStream, isTextureBound);
                if (isTextureBound)
                {
                    SerializeString(writeStream, p_pair.first);
                    SerializeStringID(writeStream, const_cast<StringID&>(p_pair.second.texture->GetName()));
                    SerializeStringID(writeStream, const_cast<StringID&>(p_pair.second.texture->GetGroup()));

                }
            }
            
            writeStream.Finish();

            uint32_t fileSize = writeStream.GetBytesProcessed();

            memcpy(((char*)&writeStream.GetBuffer()[0]) + header.resourceSizeOffset, &fileSize, sizeof(fileSize));
            dataStream.write((char*)&writeStream.GetBuffer()[0], writeStream.GetBytesProcessed());
        }
        else
        {
            CHAOS_CORE_ERROR("Cannot open scene file!");
        }

        dataStream.close();
    }

    void Resources::Material::PrepareImpl()
    {
        std::string filepath = ResourceGroupManager::Get().GetResourcePath(m_name, m_group);
        if (filepath.empty())
        {
            filepath = "Assets/Materials/";
            if (!std::filesystem::is_directory(filepath))
                std::filesystem::create_directory(filepath);
            filepath += std::string(m_name.string) + CHAOS_ASSET_EXTENSION;
        }

        std::ifstream dataStream(filepath, std::ios::binary | std::ios::in | std::ios::ate);

        if (dataStream.is_open())
        {
            std::streamsize size = dataStream.tellg();
            dataStream.seekg(0, std::ios::beg);

            std::vector<uint8_t> buffer(size);
            dataStream.read((char*)buffer.data(), size);
            dataStream.close();

            m_readStream.Start(buffer.data(), (uint32_t)size);
            ResourceHeader header;
            header.Serialize(m_readStream);
            if (header.signature != ".PNS" || header.resourceType != E_ASSETTYPE_MATERIAL || header.resourceSize != size)
            {
                m_readStream.Finish();
                throw std::exception((std::string("Asset " + m_name) + " is not valid").c_str());
            }
        }
        else 
        {
            dataStream.close();
            throw std::exception((std::string("Asset " + m_name) + " not found at location: " + filepath).c_str());
        }
    }


    void Resources::Material::LoadImpl()
    {
        StringID shaderID;
        StringID shaderGroupID;
        SerializeStringID(m_readStream, shaderID);
        SerializeStringID(m_readStream, shaderGroupID);

        SetShader(ShaderManager::Get().GetShader(shaderID, shaderGroupID));

        uint32_t uniformsCount;
        SerializeUnsignedInt(m_readStream, uniformsCount, 0, UINT32_MAX);

        while (uniformsCount--)
        {
            bool isUniformSet;
            SerializeBool(m_readStream, isUniformSet);

            if (isUniformSet)
            {
                std::string     uniformName;
                SerializeString(m_readStream, uniformName);

                uint16_t uniformType;
                SerializeUnsignedInt(m_readStream, uniformType, 0, static_cast<uint16_t>(E_ShaderType::SHADER_TYPE_MAX));

                switch (static_cast<E_ShaderType>(uniformType))
                {
                    case E_ShaderType::SHADER_TYPE_BOOL:
                    {
                        bool value;
                        SerializeBool(m_readStream, value);
                        break;
                    }
                    case E_ShaderType::SHADER_TYPE_INT:
                    {
                        int32_t value;
                        SerializeSignedInt(m_readStream, value, 0, INT32_MAX);
                        SetUniform(uniformName, value);
                        break;
                    }
                    case E_ShaderType::SHADER_TYPE_UNSIGNED_INT:
                    {
                        uint32_t value;
                        SerializeUnsignedInt(m_readStream, value, 0, UINT32_MAX);
                        SetUniform(uniformName, value);
                        break;
                    }
                    case E_ShaderType::SHADER_TYPE_FLOAT:
                    {
                        float value;
                        SerializeFloat(m_readStream, value);
                        SetUniform(uniformName, value);
                        break;
                    }
                    case E_ShaderType::SHADER_TYPE_VEC2:
                    {
                        Maths::Vector2 value;
                        SerializeFloat(m_readStream, value.x);
                        SerializeFloat(m_readStream, value.y);
                        SetUniform(uniformName, value);
                        break;
                    }
                    case E_ShaderType::SHADER_TYPE_VEC3:
                    {
                        Maths::Vector3 value;
                        SerializeFloat(m_readStream, value.x);
                        SerializeFloat(m_readStream, value.y);
                        SerializeFloat(m_readStream, value.z);
                        SetUniform(uniformName, value);
                        break;
                    }
                    case E_ShaderType::SHADER_TYPE_VEC4:
                    {
                        Maths::Vector4 value;
                        SerializeFloat(m_readStream, value.x);
                        SerializeFloat(m_readStream, value.y);
                        SerializeFloat(m_readStream, value.z);
                        SerializeFloat(m_readStream, value.w);
                        SetUniform(uniformName, value);
                        break;
                    }
                    case E_ShaderType::SHADER_TYPE_MAT3:
                    {
                        Maths::Matrix3 value;
                        for (int i = 0; i < 9; ++i)
                        {
                            SerializeFloat(m_readStream, value.elements[i]);
                        }
                        SetUniform(uniformName, value);
                        break;
                    }
                    case E_ShaderType::SHADER_TYPE_MAT4:
                    {
                        Maths::Matrix4 value;
                        for (int i = 0; i < 16; ++i)
                        {
                            SerializeFloat(m_readStream, value.elements[i]);
                        }
                        SetUniform(uniformName, value);
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        uint32_t textureUniformsCount;
        SerializeUnsignedInt(m_readStream, textureUniformsCount, 0, UINT32_MAX);

        while(textureUniformsCount--)
        {
            bool isTextureBound;
            SerializeBool(m_readStream, isTextureBound);
            if (isTextureBound)
            {
                std::string     textureName;
                StringID        textureID;
                StringID        textureGroupID;

                SerializeString(m_readStream,       textureName);

                SerializeStringID(m_readStream, textureID);
                SerializeStringID(m_readStream, textureGroupID);

                UniformValueSampler& textUniform = m_textures.at(textureName);
                textUniform.texture = TextureManager::Get().GetTexture(textureID, textureGroupID);

            }
        }

        m_readStream.Finish();
    }

    void Resources::Material::UnloadImpl()
    {
        for (std::pair<std::string, UniformValue> p_pair : m_uniforms)
        {
            if ((int64_t)p_pair.second.value.asVoidPtr != -1 &&
               (p_pair.second.type == E_ShaderType::SHADER_TYPE_VEC2 ||
                p_pair.second.type == E_ShaderType::SHADER_TYPE_VEC3 ||
                p_pair.second.type == E_ShaderType::SHADER_TYPE_VEC4 ||
                p_pair.second.type == E_ShaderType::SHADER_TYPE_MAT3 ||
                p_pair.second.type == E_ShaderType::SHADER_TYPE_MAT4))
            {
                delete p_pair.second.value.asVoidPtr;
            }
        }
        m_uniforms.clear();
        m_textures.clear();
    }
}