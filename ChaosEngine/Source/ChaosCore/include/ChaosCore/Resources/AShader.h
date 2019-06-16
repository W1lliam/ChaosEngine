#pragma once
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Resources/ShaderTypes.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"

namespace Chaos
{
    class ShaderManager;
	
	namespace Maths 
	{
		struct Matrix4;
	}
}

namespace Chaos::Resources
{
    class AShader : public AResource
    {
        friend class Chaos::ShaderManager;
    public:
        AShader(AResourceManager* p_creator, const StringID& p_name, const StringID& p_group);
		virtual ~AShader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetUniform(const std::string&& p_name, const void* p_value, E_ShaderType p_type, uint32_t p_elementCount = 1) = 0;
        virtual void SetUniform(const std::string&& p_name, const bool p_value) = 0;
        virtual void SetUniform(const std::string&& p_name, const int p_value) = 0;
        virtual void SetUniform(const std::string&& p_name, const uint32_t p_value) = 0;
        virtual void SetUniform(const std::string&& p_name, const float p_value) = 0;
        virtual void SetUniform(const std::string&& p_name, const Maths::Vector2& p_value) = 0;
        virtual void SetUniform(const std::string&& p_name, const Maths::Vector3& p_value) = 0;
        virtual void SetUniform(const std::string&& p_name, const Maths::Vector4& p_value) = 0;
		virtual void SetUniform(const std::string&& p_name, const Maths::Matrix3& p_value) = 0;
		virtual void SetUniform(const std::string&& p_name, const Maths::Matrix4& p_value) = 0;
		virtual void SetUniform(const std::string&& p_name, const std::vector<Maths::Vector3>& p_value) = 0;
		virtual void SetUniform(const std::string&& p_name, const std::vector<Maths::Vector4>& p_value) = 0;

        static AShader* Create(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group);

        virtual void PrepareImpl()      override;
        virtual void UnprepareImpl()    override;

		inline const std::vector<Uniform>& GetUniforms() { return m_uniforms; };
    
    protected:
        static constexpr    uint32_t    MAX_TEXTURE_COUNT   {256};
        static const        std::string VERTEX_DELIMITER;
        static const        std::string FRAGMENT_DELIMITER;
        static const        std::string GEOMETRY_DELIMITER;
        static              Chaos::Memory::PoolAllocator    s_allocator;

        struct ShaderProgramSource
        {
            std::string VertexSource;
            std::string FragmentSource;
            std::string GeometrySource;
        };

        ShaderProgramSource		m_sources;
		std::vector<Uniform>    m_uniforms;

		// depth state
		bool			m_depthTest					= true;
		bool			m_depthWrite				= true;

		// face culling state
		E_FaceCull		m_cullFace					= E_FaceCull::BACK;

		// blending state
		E_BlendMode		m_blendMode					= E_BlendMode::DEFAULT;

		// shadow state
		bool			m_shadowCast				= true;
		bool			m_shadowReceive				= true;

    };

    using ShaderPtr = std::shared_ptr<AShader>;
}