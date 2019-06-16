#pragma once

#include <map>
#include <string>
#include <ChaosMaths.h>
#include "ChaosCore/Resources/AShader.h"
#include "ChaosCore/Resources/ATexture.h"
#include "ChaosCore/Resources/ASkybox.h"
#include "ChaosCore/Resources/Managers/ShaderManager.h"

#ifdef CHAOS_EDITOR
namespace ChaosEditor {
    class MaterialComponentGUI; 
    class MaterialGUI;
}
#endif

namespace Chaos::Resources
{
	class Material : public AResource
	{
#ifdef CHAOS_EDITOR
		friend class ::ChaosEditor::MaterialComponentGUI;
		friend class ::ChaosEditor::MaterialGUI;
#endif

	public:
		Material(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group, ShaderPtr p_shader = nullptr);
		~Material() = default;

		inline Resources::ShaderPtr     GetShader() const                       { return m_shader;                      };
        inline Resources::TexturePtr    GetTexture(std::string p_name) const    { return m_textures.at(p_name).texture; };

		void        SetShader(Resources::ShaderPtr p_shader);
		void        SetTexture(std::string p_name, Resources::TexturePtr p_texture);
		void        SetTexture(std::string p_name, Resources::SkyboxPtr p_texture, E_ShaderType p_mapType = E_ShaderType::SHADER_TYPE_SAMPLERCUBE);
		void		SetUniform(std::string name, Maths::Matrix4& p_value);
		void		SetUniform(std::string name, Maths::Matrix3& p_value);
		void		SetUniform(std::string name, Maths::Quaternion& p_value);
		void		SetUniform(std::string name, Maths::Vector4& p_value);
		void		SetUniform(std::string name, Maths::Vector3& p_value);
		void		SetUniform(std::string name, Maths::Vector2& p_value);
		void		SetUniform(std::string name, float p_value);
		void		SetUniform(std::string name, uint32_t p_value);
		void		SetUniform(std::string name, int32_t p_value);
		void		SetUniform(std::string name, bool p_value);
		void		SetUniform(const std::string p_name, const std::vector<Maths::Vector3>& p_value);
		void		SetUniform(const std::string p_name, const std::vector<Maths::Vector4>& p_value);
		void        Bind() const;

        virtual void    Rename(const std::string& p_newName) override;
                void    Save();

	protected:
		virtual void PrepareImpl() override;
		virtual void LoadImpl() override;
		virtual void UnloadImpl() override;


	private:
        std::map<std::string, UniformValue>			m_uniforms{};
        std::map<std::string, UniformValueSampler>	m_textures{};
		Resources::ShaderPtr						m_shader;
        ReadStream                                  m_readStream;
	};

	using MaterialPtr = std::shared_ptr<Material>;
}