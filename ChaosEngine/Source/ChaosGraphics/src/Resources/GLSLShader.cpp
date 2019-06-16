#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Resources/GLSLShader.h"
#include "ChaosDebug.h"

namespace Chaos::Resources
{
    const std::string AShader::VERTEX_DELIMITER     { "#shader vertex" };
    const std::string AShader::FRAGMENT_DELIMITER   { "#shader fragment" };
    const std::string AShader::GEOMETRY_DELIMITER  { "#shader geometry" };

    Chaos::Memory::PoolAllocator AShader::s_allocator{ sizeof(Graphics::Resources::GLSLShader) * MAX_TEXTURE_COUNT, sizeof(Graphics::Resources::GLSLShader), alignof(Graphics::Resources::GLSLShader) };

    AShader* AShader::Create(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group)
    {
        return new (s_allocator.Allocate(sizeof(Graphics::Resources::GLSLShader), alignof(Graphics::Resources::GLSLShader))) Graphics::Resources::GLSLShader(p_creator, p_name, p_group);
    }
}

namespace Chaos::Graphics::Resources
{
    GLSLShader::GLSLShader(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group) : 
        AShader(p_creator, operator""_sid(p_name.c_str(), p_name.size()), p_group)
    {
    }
    void GLSLShader::Bind() const
    {
        glUseProgram(m_rendererID);
    }

    void GLSLShader::Unbind() const
    {
        glUseProgram(0);
    }

	void GLSLShader::SetUniform(const std::string && p_name, const void * p_value, E_ShaderType p_type, uint32_t p_count)
	{
		switch (p_type)
		{
		case E_ShaderType::SHADER_TYPE_BOOL:
			SetUniform(std::move(p_name), *reinterpret_cast<bool*>(&p_value));
			break;
		case E_ShaderType::SHADER_TYPE_FLOAT:
			SetUniform(std::move(p_name), *reinterpret_cast<float*>(&p_value));
			break;
		case E_ShaderType::SHADER_TYPE_INT:
			SetUniform(std::move(p_name), *reinterpret_cast<int32_t*>(&p_value));
			break;
		case E_ShaderType::SHADER_TYPE_UNSIGNED_INT:
			SetUniform(std::move(p_name), *reinterpret_cast<uint32_t*>(&p_value));
			break;
		case E_ShaderType::SHADER_TYPE_MAT3:
			SetUniform(std::move(p_name), *reinterpret_cast<const Maths::Matrix3*>(p_value));
			break;
		case E_ShaderType::SHADER_TYPE_MAT4:
			SetUniform(std::move(p_name), *reinterpret_cast<const Maths::Matrix4*>(p_value));
			break;
		case E_ShaderType::SHADER_TYPE_VEC2:
			SetUniform(std::move(p_name), *reinterpret_cast<const Maths::Vector2*>(p_value));
			break;
		case E_ShaderType::SHADER_TYPE_VEC3:
			if(p_count == 1)
				SetUniform(std::move(p_name), *reinterpret_cast<const Maths::Vector3*>(p_value));
			else if(p_count > 1)
				SetUniform(std::move(p_name), *reinterpret_cast<const std::vector<Maths::Vector3>*>(p_value));
			break;
		case E_ShaderType::SHADER_TYPE_VEC4:
			if (p_count == 1)
				SetUniform(std::move(p_name), *reinterpret_cast<const Maths::Vector4*>(p_value));
			else if(p_count > 1)
				SetUniform(std::move(p_name), *reinterpret_cast<const std::vector<Maths::Vector4>*>(p_value));
			break;
		case E_ShaderType::SHADER_TYPE_SAMPLER1D:
			SetUniform(std::move(p_name), static_cast<int>(reinterpret_cast<uintptr_t>(p_value)));
			break;
		case E_ShaderType::SHADER_TYPE_SAMPLER2D:
			SetUniform(std::move(p_name), static_cast<int>(reinterpret_cast<uintptr_t>(p_value)));
			break;
		case E_ShaderType::SHADER_TYPE_SAMPLER3D:
			SetUniform(std::move(p_name), static_cast<int>(reinterpret_cast<uintptr_t>(p_value)));
			break;
		case E_ShaderType::SHADER_TYPE_SAMPLERCUBE:
			SetUniform(std::move(p_name), static_cast<int>(reinterpret_cast<uintptr_t>(p_value)));
			break;
		default:
			break;
		}
	}

	void GLSLShader::SetUniform(const std::string&& p_name, const bool p_value)
	{
		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniform1i(location, p_value);
	}

    void GLSLShader::SetUniform(const std::string&& p_name, const int32_t p_value)
    {

		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniform1i(location, p_value);
    }

    void GLSLShader::SetUniform(const std::string&& p_name, const uint32_t p_value)
    {
        int location = GetUniformLocation(std::move(p_name));
        if (location >= 0)
            glUniform1ui(location, p_value);
    }

    void GLSLShader::SetUniform(const std::string && p_name, const float p_value)
    {

		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniform1f(location, p_value);
    }

	void GLSLShader::SetUniform(const std::string&& p_name, const Maths::Vector2& p_value)
	{
		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniform2fv(location, 1, p_value.elements);
	}

	void GLSLShader::SetUniform(const std::string&& p_name, const Maths::Vector3& p_value)
	{
		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniform3fv(location, 1, p_value.elements);
	}

	void GLSLShader::SetUniform(const std::string&& p_name, const Maths::Vector4& p_value)
	{
		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniform4fv(location, 1, p_value.elements);
	}
	void GLSLShader::SetUniform(const std::string && p_name, const Maths::Matrix3& p_value)
	{
		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniformMatrix3fv(location, 1, GL_TRUE, p_value.elements);
	}

	void GLSLShader::SetUniform(const std::string&& p_name, const Maths::Matrix4& p_value)
	{

		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniformMatrix4fv(location, 1, GL_TRUE, p_value.elements);
	}

	void GLSLShader::SetUniform(const std::string&& p_name, const std::vector<Maths::Vector3>& p_value)
	{
		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniform4fv(location, static_cast<GLsizei>(p_value.size()), reinterpret_cast<const GLfloat*>(p_value.data()));
	}

	void GLSLShader::SetUniform(const std::string && p_name, const std::vector<Maths::Vector4>& p_value)
	{
		int location = GetUniformLocation(std::move(p_name));
		if (location >= 0)
			glUniform4fv(location, static_cast<GLsizei>(p_value.size()), reinterpret_cast<const GLfloat*>(p_value.data()));
	}

    void GLSLShader::LoadImpl()
    {
        if (m_sources.VertexSource.size() > 0 && m_sources.FragmentSource.size() > 0)
        {
            const char* vtShaderChar = m_sources.VertexSource.c_str();
            const char* fragShaderChar = m_sources.FragmentSource.c_str();

            const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vtShaderChar, nullptr);
            glCompileShader(vertexShader);

			GLuint geometryShader;

			if (m_sources.GeometrySource.size() > 0)
			{
				const char* geometryChar = m_sources.GeometrySource.c_str();
				geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(geometryShader, 1, &geometryChar, nullptr);
				glCompileShader(geometryShader);
			}
			else
			{
			}

            const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragShaderChar, nullptr);
            glCompileShader(fragmentShader);

            m_rendererID = glCreateProgram();
            glAttachShader(m_rendererID, vertexShader);
			if (m_sources.GeometrySource.size() > 0)
			{
				glAttachShader(m_rendererID, geometryShader);
			}
            glAttachShader(m_rendererID, fragmentShader);

			glLinkProgram(m_rendererID);

			if (m_sources.GeometrySource.size() > 0)
			{
				glDeleteShader(geometryShader);
			}

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

			// get how many uniforms are in the shader
			int nrUniforms;
			glGetProgramiv(m_rendererID, GL_ACTIVE_UNIFORMS, &nrUniforms);
			m_uniforms.resize(nrUniforms);

			// iterate uniforms

			char buffer[128];
			for (uint16_t i = 0; i < nrUniforms; ++i)
			{
				GLenum glType;
				glGetActiveUniform(m_rendererID, i, sizeof(buffer), 0, &m_uniforms[i].size, &glType, buffer);
				m_uniforms[i].name = std::string(buffer);
				m_uniforms[i].type = static_cast<E_ShaderType>(glType);
				m_uniforms[i].location = glGetUniformLocation(m_rendererID, buffer);
			}

        }
    }

    void GLSLShader::UnloadImpl()
    {
        glDeleteProgram(m_rendererID);
    }

    int GLSLShader::GetUniformLocation(const std::string && p_name)
    {

		for (unsigned int i = 0; i < m_uniforms.size(); ++i)
		{
			if (m_uniforms[i].name == p_name)
				return m_uniforms[i].location;
		}

		CHAOS_CORE_ERROR("Uniform " + p_name + " doesn't exist");
		return -1;
    }
}