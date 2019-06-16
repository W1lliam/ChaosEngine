#pragma once
#include "ChaosCore/Resources/AShader.h"
#include "ChaosMaths.h"

namespace Chaos::Graphics::Resources
{
    class GLSLShader : public Chaos::Resources::AShader
    {
    public:

        GLSLShader(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group);
        ~GLSLShader() override = default;

        virtual void Bind() const override;
        virtual void Unbind() const override;
		virtual void SetUniform(const std::string&& p_name, const void* p_value, E_ShaderType p_type, uint32_t p_elementCount = 1) override;
        virtual void SetUniform(const std::string&& p_name, const bool p_value) override;
        virtual void SetUniform(const std::string&& p_name, const int p_value) override;
        virtual void SetUniform(const std::string&& p_name, const uint32_t p_value) override;
        virtual void SetUniform(const std::string&& p_name, const float p_value) override;
		virtual void SetUniform(const std::string&& p_name, const Maths::Vector2& p_value) override;
		virtual void SetUniform(const std::string&& p_name, const Maths::Vector3& p_value) override;
		virtual void SetUniform(const std::string&& p_name, const Maths::Vector4& p_value) override;
		virtual void SetUniform(const std::string&& p_name, const Maths::Matrix3& p_value) override;
		virtual void SetUniform(const std::string&& p_name, const Maths::Matrix4& p_value) override;
		virtual void SetUniform(const std::string&& p_name, const std::vector<Maths::Vector3>& p_value) override;
		virtual void SetUniform(const std::string&& p_name, const std::vector<Maths::Vector4>& p_value) override;
        virtual void LoadImpl()     override;
        virtual void UnloadImpl()   override;

    protected:
        int GetUniformLocation(const std::string&& p_name);

    protected:
        uint32_t m_rendererID;
        std::unordered_map<std::string, int> m_uniformLocationCache{};
    };
}