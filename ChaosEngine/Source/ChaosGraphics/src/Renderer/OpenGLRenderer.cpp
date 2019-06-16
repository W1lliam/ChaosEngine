#include "..\..\..\ChaosCore\include\ChaosCore\Rendering\IRenderer.h"
#include"ChaosGraphics/chaos_graphics_pch.h"
#include"ChaosGraphics/Renderer/OpenGLRenderer.h"
#include <ChaosDebug.h>

namespace Chaos::Graphics
{
    static bool			s_rendererInitialized	= false;

    IRenderer* IRenderer::Create()
    {
        return  new Graphics::OpenGLRenderer;
    }

	void IRenderer::Blit(Chaos::Graphics::Buffers::IFrameBuffer * p_source, Chaos::Graphics::Buffers::IFrameBuffer * p_dest, E_BufferBit p_buffer)
	{
		if (!p_source && !p_dest) return;

		if(p_source)
		{
			p_source->BindForReading();
		}
		else
		{
			glBlitFramebuffer(0, 0, p_source->GetTexture()->Width(), p_source->GetTexture()->Height(), 0, 0, ViewportWidth, ViewportHeight, p_buffer, GL_NEAREST);
		}


		if (p_dest)
		{
			p_dest->BindForDrawing();
			glBlitFramebuffer(0, 0, p_source->GetTexture()->Width(), p_source->GetTexture()->Height(), 0, 0, p_dest->GetTexture()->Width(), p_dest->GetTexture()->Height(), p_buffer, GL_NEAREST);
		}
		else
		{
			glBlitFramebuffer(0, 0, p_source->GetTexture()->Width(), p_source->GetTexture()->Height(), 0, 0, ViewportWidth, ViewportHeight, p_buffer, GL_NEAREST);
		}
	}

	void OpenGLRenderer::ResizeViewport(uint32_t p_width, uint32_t p_height)
	{
		uint16_t targetWidth	= m_targetWidth;
		uint16_t targetHeight	= m_targetHeight;

		switch (m_resizeMethod)
		{
			case E_ResizeMethod::FIT:
			{
				uint32_t width = p_width;
				uint32_t height = (uint32_t)(width / m_targetAspectRatio + 0.5f);

				if (height > p_height)
				{
					height = p_height;
					width = (uint32_t)(height * m_targetAspectRatio + 0.5f);
				}

				if (width > p_width)
				{
					width = p_width;
					height = (uint32_t)(p_height * m_targetAspectRatio + 0.5f);
				}

				uint32_t xOffset = (p_width / 2) - (width / 2);
				uint32_t yOffset = (p_height / 2) - (height / 2);

				glViewport(xOffset, yOffset, width, height);
				ViewportWidth		= width;
				ViewportHeight		= height;
				m_viewportXoffset	= xOffset;
				m_viewportYoffset	= yOffset;
				break;
			}
			case E_ResizeMethod::FILL:
			{
				uint32_t width	= p_width;
				uint32_t height = (uint32_t)(width / m_targetAspectRatio + 0.5f);

				if (height < p_height)
				{
					height	= p_height;
					width	= (uint32_t)(height * m_targetAspectRatio + 0.5f);
				}

				if(width < p_width)
				{
					width	= p_width;
					height	= (uint32_t)(width * m_targetAspectRatio + 0.5f);
				}

				uint32_t xOffset = static_cast<uint32_t>((p_width / 2.0f) - (width / 2.0f));
				uint32_t yOffset = static_cast<uint32_t>((p_height / 2.0f) - (height / 2.0f));

				glViewport(xOffset, yOffset, width, height);
				ViewportWidth		= width;
				ViewportHeight		= height;
				m_viewportXoffset	= xOffset;
				m_viewportYoffset	= yOffset;
				break;
			}
			case E_ResizeMethod::STRETCH:
				glViewport(0, 0, p_width, p_height);
				ViewportWidth		= p_width;
				ViewportHeight		= p_height;
				m_viewportXoffset	= 0;
				m_viewportYoffset	= 0;
				break;
		}

		m_targetWidth	= p_width;
		m_targetHeight	= p_height;

		m_targetAspectRatio = (float)ViewportWidth / ViewportHeight;
	}

    void OpenGLRenderer::InitImpl()
	{
		if (!s_rendererInitialized)
		{
            ViewportWidth   = m_targetWidth;
			ViewportHeight	= m_targetHeight;

			const int status		= gladLoadGL();
			CHAOS_CORE_ASSERT(status, "Failed to initialize GLAD");
			s_rendererInitialized	= true;
			CHAOS_CORE_INFO(std::string("OpenGLRenderer Version:") + (char*)(glGetString(GL_VERSION)));

            RegisterEventCallback<Events::WindowResizeEvent>(std::bind(&OpenGLRenderer::OnWindowResize, this, std::placeholders::_1));

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glEnable(GL_POINT_SMOOTH);
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			glEnable(GL_PROGRAM_POINT_SIZE);
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

#if defined(CHAOS_OPENGL) && !defined(CHAOS_SHIPPING)
			GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
			if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			{
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback(OpenGLRenderer::glDebugOutput, nullptr);
				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			}
			else
			{
				CHAOS_CORE_ERROR("Error creating Debug context");
			}
#endif
		}
	}

	void OpenGLRenderer::SetBackCull(bool p_active)
	{
		if (p_active)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else
		{
			glDisable(GL_CULL_FACE);

		}
	}

	void OpenGLRenderer::SetFrontCull(bool p_active)
	{
		if (p_active)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}
		else
		{
			glDisable(GL_CULL_FACE);

		}
	}

	void OpenGLRenderer::SetDepthTest(bool p_active)
	{
		if (p_active)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	void OpenGLRenderer::SetBlendTest(bool p_active)
	{
		if (p_active)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);

		}
	}

    void OpenGLRenderer::Clear(uint32_t p_buffer)
    {
        glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        glClear(p_buffer);
    }

    bool OpenGLRenderer::OnWindowResize(const Events::WindowResizeEvent & p_event)
    {
		ResizeViewport(p_event.GetWidth(), p_event.GetHeight());
		return false;
    }

    void OpenGLRenderer::Render(const Resources::SubMesh& p_mesh, const Resources::MaterialPtr& p_material, bool p_isWireframe)
    {
		RenderImpl(p_mesh, p_material, p_isWireframe);
    }

	void OpenGLRenderer::Render(const Resources::AnimMesh & p_Animmesh, const Resources::MaterialPtr & p_material, bool p_isWireframe)
	{
		RenderImpl(p_Animmesh, p_material, p_isWireframe);
	}

	void OpenGLRenderer::DrawLine(Maths::Vector3 p_start, float p_StartSize, Maths::Vector3 p_end, float p_endSize, Maths::Vector4 p_color)
	{
		Resources::ShaderPtr lineShader = std::reinterpret_pointer_cast<Resources::AShader>(ShaderManager::Get().CreateOrRetrieve("line", "Editor"_sid).first);
		lineShader->Bind();

		GLuint vertexArrayID;
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		GLfloat vertices[16] =
		{
			p_start.x, p_start.y, p_start.z, p_StartSize, p_color.x, p_color.y, p_color.z, p_color.w,
			p_end.x, p_end.y, p_end.z, p_endSize, p_color.x, p_color.y, p_color.z, p_color.w
		};

		GLuint vertexbuffer;
		glGenBuffers(1, &vertexbuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
		glDrawArrays(GL_LINES, 0, 2);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteVertexArrays(1, &vertexArrayID);

	}

	void OpenGLRenderer::DrawCircle(Maths::Vector3 p_origin, Maths::Vector3 p_normal, float p_radius, uint8_t p_detail, Maths::Vector4 p_color, float p_lineThickness)
	{
		Resources::ShaderPtr lineShader = std::reinterpret_pointer_cast<Resources::AShader>(ShaderManager::Get().CreateOrRetrieve("line", "Editor"_sid).first);
		lineShader->Bind();

		p_detail = std::max((uint8_t)4, p_detail);

		Maths::Quaternion quat;
		quat.MakeFromEulerAngles(p_normal, 2,1,3);

		GLuint vertexArrayID;
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		std::vector<Maths::Vector3> vertex;

		std::vector<GLfloat> vertices;

		for (int i = 0; i < p_detail; ++i)
		{
			float rad = 2.0f * 3.1415926535897932f * ((float)i / (float)p_detail);
			vertex.emplace_back(std::cosf(rad) * p_radius,std::sinf(rad) * p_radius, 0.0f);			

			vertex[i] = quat.RotateAntiClockWise(vertex[i]);
			vertex[i] += p_origin;


			vertices.push_back(vertex[i].x);
			vertices.push_back(vertex[i].y);
			vertices.push_back(vertex[i].z);

			vertices.push_back(p_lineThickness);

			vertices.push_back(p_color.x);
			vertices.push_back(p_color.y);
			vertices.push_back(p_color.z);
			vertices.push_back(p_color.w);
		}
		
		GLuint vertexbuffer;
		glGenBuffers(1, &vertexbuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
		glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)vertex.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteVertexArrays(1, &vertexArrayID);
	}

	void OpenGLRenderer::DrawPoint(Maths::Vector3 p_position, float p_size, Maths::Vector4 p_color)
	{
		Resources::ShaderPtr pointShader = std::reinterpret_pointer_cast<Resources::AShader>(ShaderManager::Get().CreateOrRetrieve("point", "Editor"_sid).first);
		pointShader->Bind();

		p_size = std::max(1.0f, p_size);

		pointShader->SetUniform("uSize", p_size);

		GLuint vertexArrayID;
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		GLfloat vertices[7] =
		{
			p_position.x, p_position.y, p_position.z, p_color.x, p_color.y, p_color.z, p_color.w
		};

		GLuint vertexbuffer;
		glGenBuffers(1, &vertexbuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
		glDrawArrays(GL_POINTS, 0, 1);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteVertexArrays(1, &vertexArrayID);
	}

	void OpenGLRenderer::RenderImpl(const Resources::SubMesh& p_mesh, const Resources::MaterialPtr& p_material, bool p_isWireframe)
	{
		if (!p_mesh.GetOwner() || p_mesh.GetOwner()->GetState() != AResource::E_ResourceState::LOADED ||
			!p_material || p_material->GetState() != AResource::E_ResourceState::LOADED ||
            !p_material->GetShader() || p_material->GetShader()->GetState() != AResource::E_ResourceState::LOADED)
		{
			return;
		}

		p_material->Bind();

		p_mesh.GetVertexArray().Bind();
		if (!p_mesh.UseIndexBuffer())
		{
			glDrawArrays(GL_TRIANGLES, 0, p_mesh.GetVertexCount());
			return;
		}
		p_mesh.GetIndexBuffer().Bind();

		if (p_isWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, p_mesh.GetIndexBuffer().Count(), GL_UNSIGNED_INT, nullptr);
	}
	void OpenGLRenderer::RenderImpl(const Resources::AnimMesh & p_Animmesh, const Resources::MaterialPtr & p_material, bool p_isWireframe)
	{
		if (p_Animmesh.GetState() != AResource::E_ResourceState::LOADED ||
			!p_material || p_material->GetState() != AResource::E_ResourceState::LOADED ||
			!p_material->GetShader() || p_material->GetShader()->GetState() != AResource::E_ResourceState::LOADED)
		{
			return;
		}

		p_material->Bind();

		p_Animmesh.GetVertexArray().Bind();
		if (!p_Animmesh.UseIndexBuffer())
		{
			glDrawArrays(GL_TRIANGLES, 0, p_Animmesh.GetVertexCount());
			return;
		}
		p_Animmesh.GetIndexBuffer().Bind();

		if (p_isWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, p_Animmesh.GetIndexBuffer().Count(), GL_UNSIGNED_INT, nullptr);
	}
    void OpenGLRenderer::glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam)
    {
        // ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::stringstream messageString;
        messageString << "---------------\n";
        messageString << "Debug message (" << id << "): " << message << '\n';

        switch (source)
        {
        case GL_DEBUG_SOURCE_API:             messageString << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   messageString << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: messageString << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     messageString << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     messageString << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           messageString << "Source: Other"; break;
        } 
        messageString << '\n';

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:               messageString << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: messageString << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  messageString << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         messageString << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         messageString << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              messageString << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          messageString << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           messageString << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               messageString << "Type: Other"; break;
        } 
        messageString << '\n';

        switch (severity)
        {
#ifdef CHAOS_PLATFORM_WINDOWS
        case GL_DEBUG_SEVERITY_HIGH:         CHAOS_CORE_FATAL("{0}Severity: high", messageString.str()); DebugBreak(); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       CHAOS_CORE_ERROR("{0}Severity: medium", messageString.str()); DebugBreak(); break;
#elif CHAOS_PLATFORM_UNIX
        case GL_DEBUG_SEVERITY_HIGH:         CHAOS_CORE_FATAL("{0}Severity: high", messageString.str()); __builtin_trap(); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       CHAOS_CORE_ERROR("{0}Severity: medium", messageString.str()); __builtin_trap(); break;
#endif
        case GL_DEBUG_SEVERITY_LOW:          CHAOS_CORE_WARN("{0}Severity: low", messageString.str()); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: CHAOS_CORE_INFO("{0}Severity: notification", messageString.str()); break;
        };
    }

	void OpenGLRenderer::SetClearColor(float p_r, float p_g, float p_b, float p_a)
	{
		m_clearColor = { p_r ,p_g ,p_b ,p_a };
	}

	void OpenGLRenderer::SetClearColor(Maths::Vector4 p_clearColor)
	{
		m_clearColor = p_clearColor;
	}
}