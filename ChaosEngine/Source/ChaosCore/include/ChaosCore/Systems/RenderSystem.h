#pragma once
#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Rendering/IRenderer.h"
#include "ChaosCore/Events/KeyEvents.h"
#include "ChaosCore/Resources/Mesh.h"
#include "ChaosCore/Resources/Material.h"
#include "ChaosCore/Scene/Scene.h"

namespace Chaos
{
    class RenderSystem : public System, public Events::EventListener
    {
        CHAOS_SYSTEM(RenderSystem);
    public:
        RenderSystem();
        ~RenderSystem() = default;

        void        Start()                 override;
        void        PreUpdate(double dt)    override;
        void        Update(double dt)		override;
        void        PostUpdate(double dt)   override;
		
		void		InitializeFBOs();
		void		ShadowPass();
		void		GeometryPass();
        void        BeautyPass();
		void		PingPongRender(Graphics::Buffers::IFrameBuffer* p_src, Graphics::Buffers::IFrameBuffer* p_dest,  Resources::MaterialPtr p_mat);
		void		BlurImage(int p_steps, Resources::TexturePtr p_texture);
		void		BloomPass();

        uint32_t    GetDrawCallCount()      const;

        inline void SetFinalOutputBuffer(uint32_t p_bufferID) { m_finalOutputBuffer = p_bufferID; };

        static System& Load(ReadStream & p_stream);

    private:
        bool OnKeyPressed(const Events::KeyPressedEvent& p_event);

        std::vector<Maths::Matrix4> m_shadowsLightSpace     {};
		Resources::Mesh	            m_quadRender;
        Resources::MaterialPtr	    m_quadMaterial          {nullptr};
        ScenePtr                    m_currentScene          {nullptr};
        uint32_t                    m_finalOutputBuffer     {0};
        uint32_t                    m_drawCallCount         {0};
        bool                        m_useWireframe          {false};
		bool					    m_fxaa				    {false};


    };
}