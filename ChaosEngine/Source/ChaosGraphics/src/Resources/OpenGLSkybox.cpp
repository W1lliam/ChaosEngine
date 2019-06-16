#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Resources/OpenGLSkybox.h"
#include "ChaosCore/Resources/Managers/ShaderManager.h"
#include "ChaosCore/Resources/Managers/MaterialManager.h"
#include "ChaosCore/Rendering/IRenderer.h"

namespace Chaos::Resources
{
    Memory::PoolAllocator ASkybox::s_allocator { sizeof(Resources::OpenGLSkybox) * MAX_TEXTURE_COUNT, sizeof(Resources::OpenGLSkybox), alignof(Resources::OpenGLSkybox) };

    ASkybox* ASkybox::Create(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group)
    {
        return new (s_allocator.Allocate(sizeof(Resources::OpenGLSkybox), alignof(Resources::OpenGLSkybox))) OpenGLSkybox(p_creator, p_id, p_group);
    }

    ASkybox* ASkybox::Create(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group, uint32_t& p_width, uint32_t& p_height)
	{
		return new (s_allocator.Allocate(sizeof(Resources::OpenGLSkybox), alignof(Resources::OpenGLSkybox))) OpenGLSkybox(p_creator, p_id, p_group, p_width, p_height);
	}

    OpenGLSkybox::OpenGLSkybox(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group) :
        ASkybox(p_creator, p_id, p_group)
    {
    }

    OpenGLSkybox::OpenGLSkybox(Chaos::AResourceManager* p_creator, const std::string& p_id, const StringID& p_group, uint32_t& p_width, uint32_t& p_height) :
        ASkybox(p_creator, p_id, p_group)
	{
        m_environmentSize = p_width;
		PostLoadImpl();
	}

    void OpenGLSkybox::PostLoadImpl()
    {
		if (m_skyboxData.environmentMap.size() > 0)
		{
            //Load environment map
            glGenTextures(1, &m_environmentMapID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMapID);
            const uint8_t envMipMapCount = static_cast<uint8_t>(1 + floor(std::log2(m_environmentSize)));
            uint32_t floatsRead = 0;
            for (uint8_t mip = 0; mip < envMipMapCount; mip++)
            {
                unsigned int mipSize = static_cast<unsigned int>(m_environmentSize * std::pow(0.5, mip));
                for (unsigned int i = 0; i < 6; ++i)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, GL_RGB16F, mipSize, mipSize, 0, GL_RGB, GL_FLOAT, &m_skyboxData.environmentMap[floatsRead]);
                    floatsRead += mipSize * mipSize * 3;
                }
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


            //Load Irradiance
            glGenTextures(1, &m_irradianceMapID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMapID);
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, &m_skyboxData.irradianceMap[i * 32 * 32 * 3]);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //Prefiltered
            glGenTextures(1, &m_preFilteredMapID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_preFilteredMapID);
            const uint8_t prefilteredMipMapCount = static_cast<uint8_t>(1 + floor(std::log2(m_prefilteredSize)));
            floatsRead = 0;
            for (uint8_t mip = 0; mip < prefilteredMipMapCount; mip++)
            {
                unsigned int mipSize = static_cast<unsigned int>(m_prefilteredSize * std::pow(0.5, mip));
                for (unsigned int i = 0; i < 6; ++i)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, GL_RGB16F, mipSize, mipSize, 0, GL_RGB, GL_FLOAT, &m_skyboxData.prefilterMap[floatsRead]);
                    floatsRead += mipSize * mipSize * 3;
                }
            }


            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         }

		m_state.store(E_ResourceState::LOADED);

    }

    void OpenGLSkybox::UnloadImpl()
    {

        glDeleteTextures(1, &m_preFilteredMapID);
        glDeleteTextures(1, &m_irradianceMapID);
        glDeleteTextures(1, &m_environmentMapID);


        if (s_cubeVAO != 0)
        {
            glDeleteVertexArrays(1, &s_cubeVAO);
            glDeleteBuffers(1, &s_cubeVBO);
            s_cubeVAO = 0;
            s_cubeVBO = 0;
        }
        if (s_quadVAO != 0)
        {
            glDeleteVertexArrays(1, &s_quadVAO);
            glDeleteBuffers(1, &s_quadVBO);
            s_quadVAO = 0;
            s_quadVBO = 0;
        }

    }

	uint32_t& OpenGLSkybox::GetTextureID()
	{
		return m_environmentMapID;
	}

    void OpenGLSkybox::Bind(const uint32_t p_slot) const
    {
        glActiveTexture(GL_TEXTURE0 + p_slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMapID);
    }

    void OpenGLSkybox::BindIrradiance(const uint32_t p_slot) const
    {
        glActiveTexture(GL_TEXTURE0 + p_slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMapID);
    }

    void OpenGLSkybox::BindPrefilter(const uint32_t p_slot) const
    {
        glActiveTexture(GL_TEXTURE0 + p_slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_preFilteredMapID);
    }

    void OpenGLSkybox::Unbind() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void OpenGLSkybox::Render()
    {

        glDepthFunc(GL_LEQUAL);
        RenderCube();
        glDepthFunc(GL_LESS);
    }

    std::vector<float>   ASkybox::GenerateBRDF()
    {
        uint32_t captureFBO = 0;
        uint32_t captureRBO = 0;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        uint32_t BRDFMap = OpenGLSkybox::ConvoluteBRDFMap(captureFBO, captureRBO);
        std::vector<float> finalBRDF;


        //Get BRDFMap
        glBindTexture(GL_TEXTURE_2D, BRDFMap);
        finalBRDF.resize(512 * 512 * 2);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, &finalBRDF[0]);

        glBindTexture(GL_TEXTURE_2D, 0);

        glDeleteTextures(1, &BRDFMap);
        glDeleteFramebuffers(1, &captureFBO);
        glDeleteRenderbuffers(1, &captureRBO);

        glViewport(Graphics::IRenderer::Get().GetViewportXOffset(), Graphics::IRenderer::Get().GetViewportYOffset(), Graphics::IRenderer::Get().ViewportWidth, Graphics::IRenderer::Get().ViewportHeight);

        return finalBRDF;
    }
    SkyboxData ASkybox::PrecomputeSkybox(float* p_data, uint32_t p_width, uint32_t p_height, uint8_t p_bpp, uint32_t p_environmentMapSize, uint32_t p_prefilteredMapSize)
    {
        SkyboxData finalSkybox;
        if (p_data)
        {
            uint32_t rectMapID = 0;
            //Load HDR environment map
            glGenTextures(1, &rectMapID);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rectMapID);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGB, GL_FLOAT, p_data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


            //Setup Framebuffer
            uint32_t captureFBO = 0;
            uint32_t captureRBO = 0;
            glGenFramebuffers(1, &captureFBO);
            glGenRenderbuffers(1, &captureRBO);

            uint32_t environmentMap = OpenGLSkybox::CubeMapFromEquirectangular(captureFBO, captureRBO, rectMapID, p_environmentMapSize);
            uint32_t irradianceMap  = OpenGLSkybox::ConvoluteIrradianceMap(captureFBO, captureRBO, environmentMap);
            uint32_t preFilteredMap = OpenGLSkybox::ConvolutePreFilteredMap(captureFBO, captureRBO, environmentMap, p_prefilteredMapSize);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            //Get EnvironmentMap
            glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);
            const uint8_t envMipMapCount = static_cast<uint8_t>(1 + floor(std::log2(p_environmentMapSize)));
            finalSkybox.environmentMap.resize(p_environmentMapSize * p_environmentMapSize * sizeof(float) * 6 * 2);
            uint32_t floatsWritten = 0;
            for (uint8_t mip = 0; mip < envMipMapCount; mip++)
            {
                unsigned int mipSize = static_cast<unsigned int>(p_environmentMapSize * std::pow(0.5, mip));
                for (unsigned int i = 0; i < 6; ++i)
                {
                    glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, GL_RGB, GL_FLOAT, &finalSkybox.environmentMap[floatsWritten]);
                    floatsWritten += mipSize * mipSize * 3;
                }
            }
            finalSkybox.environmentMap.resize(floatsWritten);

            //Get IrradianceMap
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            finalSkybox.irradianceMap.resize(32 * 32 * 3 * 6);
            floatsWritten = 0;
            for (unsigned int i = 0; i < 6; ++i)
            {
                glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, GL_FLOAT, &finalSkybox.irradianceMap[floatsWritten]);
                floatsWritten += 32 * 32 * 3;
            }


            //Get PrefilterMap
            glBindTexture(GL_TEXTURE_CUBE_MAP, preFilteredMap);
            const uint8_t preFilterMipMapCount = static_cast<uint8_t>(1 + floor(std::log2(p_prefilteredMapSize)));
            finalSkybox.prefilterMap.resize(p_prefilteredMapSize * p_prefilteredMapSize * sizeof(float) * 6 * 2);
            floatsWritten = 0;
            for (uint8_t mip = 0; mip < preFilterMipMapCount; mip++)
            {
                unsigned int mipSize = static_cast<unsigned int>(p_prefilteredMapSize * std::pow(0.5, mip));
                for (unsigned int i = 0; i < 6; ++i)
                {
                    glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, GL_RGB, GL_FLOAT, &finalSkybox.prefilterMap[floatsWritten]);
                    floatsWritten += mipSize * mipSize * 3;
                }
            }
            finalSkybox.prefilterMap.resize(floatsWritten);

            glDeleteTextures(1, &environmentMap);
            glDeleteTextures(1, &irradianceMap);
            glDeleteTextures(1, &preFilteredMap);

            glDeleteFramebuffers(1, &captureFBO);
            glDeleteRenderbuffers(1, &captureRBO);
            glViewport(Graphics::IRenderer::Get().GetViewportXOffset(), Graphics::IRenderer::Get().GetViewportYOffset(), Graphics::IRenderer::Get().ViewportWidth, Graphics::IRenderer::Get().ViewportHeight);
        }
        return finalSkybox;
    }

    uint32_t OpenGLSkybox::CubeMapFromEquirectangular(uint32_t p_captureFBO, uint32_t p_captureRBO, uint32_t p_rectMap, uint32_t p_environmentMapSize)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, p_captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, p_captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, p_environmentMapSize, p_environmentMapSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, p_captureRBO);


        //Setup Cubemap & attach to FBO
        uint32_t environmentMapID = 0;
        glGenTextures(1, &environmentMapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMapID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, p_environmentMapSize, p_environmentMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //convert HDR equirectangular environment map to cubemap equivalent
        ShaderPtr rectToCubeMapShader = ShaderManager::Get().GetShader("EquirectangularToCubemap"_sid, "DefaultResources"_sid);
        rectToCubeMapShader->Bind();
        rectToCubeMapShader->SetUniform("uEquirectangularMap", 0);
        rectToCubeMapShader->SetUniform("uProjection", s_captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, p_rectMap);

        glViewport(0, 0, p_environmentMapSize, p_environmentMapSize); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, p_captureFBO);
        for (unsigned int i = 0; i < 6; ++i)
        {
            rectToCubeMapShader->SetUniform("uView", s_captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentMapID, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            RenderCube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMapID);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        return environmentMapID;
    }

    uint32_t OpenGLSkybox::ConvoluteIrradianceMap(uint32_t p_captureFBO, uint32_t p_captureRBO, uint32_t p_environmentMap)
    {
        //Sample Irradiance map
        uint32_t irradianceMapID = 0;
        glGenTextures(1, &irradianceMapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        glBindFramebuffer(GL_FRAMEBUFFER, p_captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, p_captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);


        ShaderPtr irradianceSamplingShader = ShaderManager::Get().GetShader("IrradianceSampling"_sid, "DefaultResources"_sid);
        irradianceSamplingShader->Bind();
        irradianceSamplingShader->SetUniform("uEnvironmentMap", 0);
        irradianceSamplingShader->SetUniform("uProjection", s_captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, p_environmentMap);

        glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, p_captureFBO);
        for (unsigned int i = 0; i < 6; ++i)
        {
            irradianceSamplingShader->SetUniform("uView", s_captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMapID, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            RenderCube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return irradianceMapID;
    }

    uint32_t OpenGLSkybox::ConvolutePreFilteredMap(uint32_t p_captureFBO, uint32_t p_captureRBO, uint32_t p_environmentMap, uint32_t p_prefilteredMapSize)
    {
        uint32_t    preFilteredMapID = 0;
        glGenTextures(1, &preFilteredMapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, preFilteredMapID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, p_prefilteredMapSize, p_prefilteredMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        ShaderPtr preFilteredSamplingShader = ShaderManager::Get().GetShader("PreFilteredSampling"_sid, "DefaultResources"_sid);
        preFilteredSamplingShader->Bind();
        preFilteredSamplingShader->SetUniform("uEnvironmentMap", 0);
        preFilteredSamplingShader->SetUniform("uProjection", s_captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, p_environmentMap);

        glBindFramebuffer(GL_FRAMEBUFFER, p_captureFBO);
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
        {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth = static_cast<unsigned int>(p_prefilteredMapSize * std::pow(0.5, mip));
            unsigned int mipHeight = static_cast<unsigned int>(p_prefilteredMapSize * std::pow(0.5, mip));
            glBindRenderbuffer(GL_RENDERBUFFER, p_captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            preFilteredSamplingShader->SetUniform("uRoughness", roughness);
            for (unsigned int i = 0; i < 6; ++i)
            {
                preFilteredSamplingShader->SetUniform("uView", s_captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, preFilteredMapID, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                RenderCube();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return preFilteredMapID;
    }

    uint32_t OpenGLSkybox::ConvoluteBRDFMap(uint32_t p_captureFBO, uint32_t p_captureRBO)
    {
        uint32_t BRDFMapID = 0;
        glGenTextures(1, &BRDFMapID);

        // pre-allocate enough memory for the LUT texture.
        glBindTexture(GL_TEXTURE_2D, BRDFMapID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
        // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
        glBindFramebuffer(GL_FRAMEBUFFER, p_captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, p_captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BRDFMapID, 0);

        glViewport(0, 0, 512, 512);
        ShaderPtr BRDFSamplingShader = ShaderManager::Get().GetShader("BRDFSampling"_sid, "DefaultResources"_sid);
        BRDFSamplingShader->Bind();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return BRDFMapID;
    }


    void OpenGLSkybox::RenderQuad()
    {
        if (s_quadVAO == 0)
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.5f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.5f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.5f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.5f, 1.0f, 0.0f
            };
            // setup plane VAO
            glGenVertexArrays(1, &s_quadVAO);
            glGenBuffers(1, &s_quadVBO);
            glBindVertexArray(s_quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, s_quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(s_quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void OpenGLSkybox::RenderCube()
    {
        // initialize (if necessary)
        if (s_cubeVAO == 0)
        {
            float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                // right face
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
            };
            glGenVertexArrays(1, &s_cubeVAO);
            glGenBuffers(1, &s_cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, s_cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(s_cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        // render Cube
        glBindVertexArray(s_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
}