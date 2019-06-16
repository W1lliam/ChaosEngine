#pragma once
#include "ChaosCore/Resources/ASkybox.h"
#include "ChaosCore/chaos_core_pch.h"

namespace Chaos::Resources
{   

    class OpenGLSkybox : public ASkybox
    {
    public:
        OpenGLSkybox(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group);
        OpenGLSkybox(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group, uint32_t& p_width, uint32_t& p_height);
        virtual ~OpenGLSkybox() = default;

        virtual void PostLoadImpl()     override;
        virtual void UnloadImpl()       override;

		virtual uint32_t& GetTextureID() override;

        virtual void Bind(const uint32_t p_slot = 0)                const   override;
        virtual void BindIrradiance(const uint32_t p_slot = 0)      const   override;
        virtual void BindPrefilter(const uint32_t p_slot = 0)       const   override;
        virtual void Unbind()                                       const   override;
        virtual void Render()                                               override;

    
        static uint32_t CubeMapFromEquirectangular(uint32_t p_captureFBO, uint32_t p_captureRBO, uint32_t p_rectMap, uint32_t p_environmentMapSize);
        static uint32_t ConvoluteIrradianceMap(uint32_t p_captureFBO, uint32_t p_captureRBO, uint32_t p_environmentMap);
        static uint32_t ConvolutePreFilteredMap(uint32_t p_captureFBO, uint32_t p_captureRBO, uint32_t p_environmentMap, uint32_t p_prefilteredMapSize);
        static uint32_t ConvoluteBRDFMap(uint32_t p_captureFBO, uint32_t p_captureRBO);

    protected:
        
        static void RenderQuad();
        static void RenderCube();

    protected:
        uint32_t m_environmentMapID         {0};
        uint32_t m_irradianceMapID          {0};
        uint32_t m_preFilteredMapID         {0};

        inline static uint32_t s_quadVBO    {0};
        inline static uint32_t s_quadVAO    {0};

        inline static uint32_t s_cubeVBO    {0};
        inline static uint32_t s_cubeVAO    {0};

        inline static const Maths::Matrix4 s_captureProjection  = Maths::Matrix4::CreatePerspective(90.0f, 1.0f, 0.1f, 10.0f);
        inline static const Maths::Matrix4 s_captureViews[]     =
        {
            Maths::Matrix4::CreateLookAt(0.0f, 0.0f, 0.0f,       1.0f,  0.0f,  0.0f,        0.0f, -1.0f,  0.0f),
            Maths::Matrix4::CreateLookAt(0.0f, 0.0f, 0.0f,      -1.0f,  0.0f,  0.0f,        0.0f, -1.0f,  0.0f),
            Maths::Matrix4::CreateLookAt(0.0f, 0.0f, 0.0f,       0.0f,  1.0f,  0.0f,        0.0f,  0.0f,  1.0f),
            Maths::Matrix4::CreateLookAt(0.0f, 0.0f, 0.0f,       0.0f, -1.0f,  0.0f,        0.0f,  0.0f, -1.0f),
            Maths::Matrix4::CreateLookAt(0.0f, 0.0f, 0.0f,       0.0f,  0.0f,  1.0f,        0.0f, -1.0f,  0.0f),
            Maths::Matrix4::CreateLookAt(0.0f, 0.0f, 0.0f,       0.0f,  0.0f, -1.0f,        0.0f, -1.0f,  0.0f)
        };


    };
}