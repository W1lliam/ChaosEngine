#pragma once
#include "ChaosCore/Resources/SubMesh.h"
#include "ChaosCore/Resources/Material.h"
#include "ChaosCore/Utils/Singleton.h"
#include "ChaosCore/Resources/Buffers/IFrameBuffer.h"
#include "ChaosCore/Resources/AnimationMesh.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
	class Transform;
}

namespace Chaos::Graphics
{   
    class IWindow;

	enum E_BufferBit : uint32_t
	{
		COLOR_BIT	= 0x00004000,
		DEPTH_BIT	= 0x00000100,
		STENCIL_BIT = 0x00000400
	};

	enum class E_ResizeMethod : uint8_t
	{
		FIT,
		FILL,
		STRETCH
	};

    class IRenderer : public Singleton<IRenderer>
    {
    public:
        virtual ~IRenderer() = default;

        inline	static  void		Init() { Get().InitImpl();  Expose(); }
				virtual void        Clear(uint32_t p_buffer = COLOR_BIT | DEPTH_BIT)															= 0;
				virtual void		SetBackCull(bool p_active = true)																			= 0;
				virtual void		SetFrontCull(bool p_active = false)																			= 0;
				virtual void		SetDepthTest(bool p_active = true)																			= 0;
				virtual void		SetBlendTest(bool p_active)																					= 0;
				virtual void        Render(const Resources::SubMesh& p_mesh, const Resources::MaterialPtr& p_material, bool p_isWireframe)		= 0;
				virtual void        Render(const Resources::AnimMesh& p_Animmesh, const Resources::MaterialPtr& p_material, bool p_isWireframe)	= 0;
				virtual void		ResizeViewport(uint32_t p_width, uint32_t p_height)															= 0;

				static  IRenderer*  Create();
				virtual  void		DrawLine(Maths::Vector3 p_start, float p_StartSize, Maths::Vector3 p_end, float p_endSize, Maths::Vector4 p_color)										= 0;
				virtual  void		DrawCircle(Maths::Vector3 p_origin, Maths::Vector3 p_normal, float p_radius, uint8_t p_detail, Maths::Vector4 p_color, float p_lineThickness)			= 0;
				virtual  void		DrawPoint(Maths::Vector3 p_position, float p_size, Maths::Vector4 p_color)																				= 0;
				virtual void		SetClearColor(Maths::Vector4 p_clearColor)																												= 0;
				virtual void		SetClearColor(float p_r, float p_g, float p_b, float p_a)																								= 0;

				void				Blit(Chaos::Graphics::Buffers::IFrameBuffer* p_source, Chaos::Graphics::Buffers::IFrameBuffer* p_dest = nullptr, E_BufferBit p_buffer = COLOR_BIT);
				void				Blit(Chaos::Graphics::Buffers::IFrameBuffer* p_source, uint32_t p_destBuffer, E_BufferBit p_buffer = COLOR_BIT);

				inline  void		SetTargetWidth(uint32_t p_width)				{ ViewportWidth		= p_width;	m_targetAspectRatio = ViewportWidth / (float)ViewportHeight;};
                inline  void		SetTargetHeight(uint32_t p_height)				{ ViewportHeight	= p_height; m_targetAspectRatio = ViewportWidth / (float)ViewportHeight;};

				inline  void        SetResizeMethod(E_ResizeMethod p_resizeMethod)	{ m_resizeMethod = p_resizeMethod; };

                inline  uint32_t	GetWidth()					const				{return m_targetWidth;};
                inline  uint32_t	GetHeight()					const				{return m_targetHeight;};


				inline  uint32_t	GetViewportXOffset()		const				{ return m_viewportXoffset; };
				inline  uint32_t	GetViewportYOffset()		const				{ return m_viewportYoffset; };

				Chaos::Graphics::Buffers::IFrameBuffer* RenderTexture				{ nullptr };
				Chaos::Graphics::Buffers::IFrameBuffer* ShadowBuffer				{ nullptr };
				Chaos::Graphics::Buffers::IFrameBuffer* GBuffer						{ nullptr };


				Chaos::Graphics::Buffers::IFrameBuffer* PingBuffer					{ nullptr };
				Chaos::Graphics::Buffers::IFrameBuffer* PongBuffer					{ nullptr };

                inline  static  void      Expose()
                {
                    ssq::Table rendererTable = ScriptsHandler::Get().GetVirtualMachine().addTable("Renderer");
                    rendererTable.addFunc("GetScreenWidth", []() {   return IRenderer::Get().GetWidth();              });
                    rendererTable.addFunc("GetScreenHeight", []() {   return IRenderer::Get().GetHeight();             });
                }

	public:
		uint32_t		ViewportWidth		{1280};
		uint32_t		ViewportHeight		{720};

	protected:
		virtual void InitImpl() = 0;
    protected:
        uint32_t		m_targetWidth		{1280};
        uint32_t		m_targetHeight	    {720};
        uint32_t		m_viewportXoffset   {0};
        uint32_t		m_viewportYoffset   {0};	
		E_ResizeMethod  m_resizeMethod		{ E_ResizeMethod::FIT };
        float			m_targetAspectRatio { ViewportWidth / (float)ViewportHeight };
    };
}