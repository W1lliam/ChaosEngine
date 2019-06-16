#pragma once
#include "ChaosCore.h"

namespace Chaos::Graphics
{
    class OpenGLRenderer : public IRenderer, public Events::EventListener
    {
    public:
        OpenGLRenderer() = default;
        ~OpenGLRenderer() = default;
		virtual void InitImpl() override;
        
		virtual void SetBackCull	(bool p_active	= true)						override;
		virtual void SetFrontCull	(bool p_active	= false)					override;
		virtual void SetDepthTest	(bool p_active)								override;
		virtual void SetBlendTest	(bool p_active)								override;

		virtual void Clear(uint32_t p_buffer = COLOR_BIT | DEPTH_BIT) override;
		virtual void SetClearColor(float p_r, float p_g, float p_b, float p_a);
		virtual void SetClearColor(Maths::Vector4 p_clearColor);

		bool OnWindowResize(const Events::WindowResizeEvent& p_event);

        virtual void Render(const Resources::SubMesh& p_mesh, const Resources::MaterialPtr& p_material, bool p_isWireframe) override;
		virtual void Render(const Resources::AnimMesh& p_Animmesh, const Resources::MaterialPtr& p_material, bool p_isWireframe) override;
		virtual void DrawLine(Maths::Vector3 p_start, float p_StartSize, Maths::Vector3 p_end, float p_endSize, Maths::Vector4 p_color) override;
		virtual void DrawCircle(Maths::Vector3 p_origin, Maths::Vector3 p_normal, float p_radius, uint8_t p_detail, Maths::Vector4 p_color, float p_lineThickness) override;
		virtual void DrawPoint(Maths::Vector3 p_position, float p_size, Maths::Vector4 p_color) override;

		virtual void ResizeViewport(uint32_t p_width, uint32_t p_height) override;

        static void RenderImpl(const Resources::SubMesh& p_mesh, const Resources::MaterialPtr& p_material, bool p_isWireframe);
		static void RenderImpl(const Resources::AnimMesh& p_Animmesh, const Resources::MaterialPtr& p_material, bool p_isWireframe);

        static void glDebugOutput(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam);

	private:
		Chaos::Maths::Vector4 m_clearColor{ 0.3f, 0.3f, 0.3f, 1.0f };
    };
}