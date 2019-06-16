#include "ChaosCore/Systems/RenderSystem.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Rendering/IRenderer.h"

#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"

#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Components/MeshComponent.h"
#include "ChaosCore/Components/MaterialComponent.h"
#include "ChaosCore/Components/LightComponent.h"
#include "ChaosCore/Components/AnimationComponent.h"

#include "ChaosCore/Resources/Managers/MeshManager.h"
#include "ChaosCore/Resources/Managers/ShaderManager.h"
#include "ChaosCore/Resources/Managers/MaterialManager.h"
#include "ChaosCore/Resources/Managers/TextureManager.h"
#include "ChaosCore/Resources/Managers/AnimationManager.h"
#include "ChaosCore/Resources/Material.h"

#include "ChaosCore/Systems/LightSystem.h"

#include "ChaosCore/TransformBase.h"

namespace Chaos
{
    CHAOS_DEFINE_SYSTEM_TYPEID(RenderSystem);

    RenderSystem::RenderSystem() : System(System::NORMAL_SYSTEM_PRIORITY, E_SystemExecutionModeFlags::BOTH), m_quadRender(nullptr, "RenderQuad", "RenderGroup"_sid)
    {
        RegisterEventCallback<Events::KeyPressedEvent>(std::bind(&RenderSystem::OnKeyPressed, this, std::placeholders::_1));
        m_quadRender.MakeAsQuad();

		InitializeFBOs();
    }

	void RenderSystem::InitializeFBOs()
	{
		if (!Graphics::IRenderer::Get().GBuffer)
		{

			Graphics::IRenderer::Get().GBuffer = Chaos::Graphics::Buffers::IFrameBuffer::Create({ TextureManager::Get().CreateTexture("GPosition", "RenderGroup"_sid,			Graphics::IRenderer::Get().ViewportWidth,	Graphics::IRenderer::Get().ViewportHeight,	E_TextureType::RGB16F),
																								  TextureManager::Get().CreateTexture("GNormal", "RenderGroup"_sid,				Graphics::IRenderer::Get().ViewportWidth,	Graphics::IRenderer::Get().ViewportHeight,	E_TextureType::RGB16F),
																								  TextureManager::Get().CreateTexture("GAlbedo", "RenderGroup"_sid,				Graphics::IRenderer::Get().ViewportWidth,	Graphics::IRenderer::Get().ViewportHeight,	E_TextureType::RGBA8),
																								  TextureManager::Get().CreateTexture("GMetallicRoughness", "RenderGroup"_sid,	Graphics::IRenderer::Get().ViewportWidth,	Graphics::IRenderer::Get().ViewportHeight,	E_TextureType::RGBA8),
																								  TextureManager::Get().CreateTexture("GEmissive", "RenderGroup"_sid,			Graphics::IRenderer::Get().ViewportWidth,	Graphics::IRenderer::Get().ViewportHeight,	E_TextureType::RGBA8),
																								  TextureManager::Get().CreateTexture("GUnlit", "RenderGroup"_sid,				Graphics::IRenderer::Get().ViewportWidth,	Graphics::IRenderer::Get().ViewportHeight,	E_TextureType::RGBA8),
																								  TextureManager::Get().CreateTexture("GDepth", "RenderGroup"_sid,				Graphics::IRenderer::Get().ViewportWidth,	Graphics::IRenderer::Get().ViewportHeight,	E_TextureType::DEPTH24) });
		}

		if (!Graphics::IRenderer::Get().RenderTexture)
		{
			Graphics::IRenderer::Get().RenderTexture = Chaos::Graphics::Buffers::IFrameBuffer::Create({TextureManager::Get().CreateTexture("RenderTexture", "RenderGroup"_sid, Graphics::IRenderer::Get().ViewportWidth, Graphics::IRenderer::Get().ViewportHeight, E_TextureType::RGB16F),
																									  TextureManager::Get().CreateTexture("BrightPass", "RenderGroup"_sid, Graphics::IRenderer::Get().ViewportWidth, Graphics::IRenderer::Get().ViewportHeight, E_TextureType::RGB16F) });
		}

		if (!Graphics::IRenderer::Get().ShadowBuffer)
		{
			Graphics::IRenderer::Get().ShadowBuffer = Chaos::Graphics::Buffers::IFrameBuffer::Create(TextureManager::Get().CreateTexture("ShadowTexture", "RenderGroup"_sid, Graphics::IRenderer::Get().ViewportWidth, Graphics::IRenderer::Get().ViewportHeight, E_TextureType::DEPTH));
		}

		if (!Graphics::IRenderer::Get().PingBuffer)
		{
			Graphics::IRenderer::Get().PingBuffer = Chaos::Graphics::Buffers::IFrameBuffer::Create(TextureManager::Get().CreateTexture("PingTexture", "RenderGroup"_sid, Graphics::IRenderer::Get().ViewportWidth, Graphics::IRenderer::Get().ViewportHeight, E_TextureType::RGB16F));
		}
		if (!Graphics::IRenderer::Get().PongBuffer)
		{
			Graphics::IRenderer::Get().PongBuffer = Chaos::Graphics::Buffers::IFrameBuffer::Create(TextureManager::Get().CreateTexture("PongTexture", "RenderGroup"_sid, Graphics::IRenderer::Get().ViewportWidth, Graphics::IRenderer::Get().ViewportHeight, E_TextureType::RGB16F));
		}
	}

    System& RenderSystem::Load(ReadStream & p_stream)
    {
        RenderSystem* system = SystemManager::Get().AddSystem<RenderSystem>();
        static_cast<System*>(system)->Load(p_stream);
        return *system;
    }

    bool RenderSystem::OnKeyPressed(const Events::KeyPressedEvent& p_event)
    {
        switch (p_event.KeyCode())
        {
        case E_ChaosKeyCode::CHAOS_KEY_F1 :
            m_useWireframe = !m_useWireframe;
            break;
		case E_ChaosKeyCode::CHAOS_KEY_X:
			m_fxaa = !m_fxaa;
			break;
        default:
            break;
        }
        return false;
    }

    void RenderSystem::Start()
    {
		Graphics::IRenderer::Get().SetBlendTest(false);
    }

    void RenderSystem::PreUpdate(double dt)
    {
		Graphics::IRenderer::Get().SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        m_drawCallCount = 0;
        m_currentScene = m_systemManager->GetSystem<SceneSystem>()->GetCurrentScene();
    }
	void RenderSystem::Update(double dt)
	{
        ShadowPass();
	}

    void RenderSystem::PostUpdate(double dt)
    {
		//---------------Geometry Pass------------------------


        GeometryPass();

		//---------------Beauty Pass------------------------

        BeautyPass();

		// PING-PONG Begin Loop

		BlurImage(m_currentScene->GetRenderParams().bloomBlurPasses * 2, TextureManager::Get().GetTexture("BrightPass"_sid, "RenderGroup"_sid));

		// PING-PONG End Loop

		Graphics::IRenderer::Get().Blit(Graphics::IRenderer::Get().RenderTexture, Graphics::IRenderer::Get().PongBuffer);
		Graphics::IRenderer::Get().RenderTexture->Unbind();
		Graphics::IRenderer::Get().PongBuffer->Unbind();

		BloomPass();


        if (m_finalOutputBuffer != 0)
        {
            Chaos::Graphics::Buffers::IFrameBuffer::Bind(m_finalOutputBuffer);
        }

		Graphics::IRenderer::Get().Clear(Graphics::COLOR_BIT);
		
        m_quadMaterial->SetShader(ShaderManager::Get().GetShader("FXAA"_sid));
        m_quadMaterial->SetUniform("uGamma", m_currentScene->GetRenderParams().gamma);
        m_quadMaterial->SetTexture("screenTex", TextureManager::Get().GetTexture("RenderTexture"_sid, "RenderGroup"_sid));
		Graphics::IRenderer::Get().Render(*m_quadRender.GetSubMeshes()[0], m_quadMaterial, false);
        m_drawCallCount++;

    }

    void RenderSystem::ShadowPass()
    {
        Resources::MaterialPtr shadowMtl = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("Shadow", "RenderGroup"_sid).first);
        shadowMtl->SetShader(ShaderManager::Get().GetShader("depth"_sid));

        Graphics::IRenderer::Get().ShadowBuffer->Bind();
        Graphics::IRenderer::Get().Clear();
        Graphics::IRenderer::Get().SetDepthTest(true);
        m_shadowsLightSpace.clear();

        for(auto lightIt = m_currentScene->begin<LightComponent>(); lightIt != m_currentScene->end<LightComponent>(); lightIt++)
        {
            LightComponent* lightComp = static_cast<LightComponent*>(lightIt->second);

            if (lightComp->CastShadows())
            {
                const ShadowInfo&   lightShadow = lightComp->GetShadowInfo();
                Maths::Matrix4      lightSpaceMatrix = lightShadow.projection * lightShadow.view;
                m_shadowsLightSpace.push_back(lightSpaceMatrix);
                shadowMtl->SetUniform("uLightSpaceMatrix", lightSpaceMatrix);

                Graphics::IRenderer::Get().SetFrontCull(true);
                Graphics::IRenderer::Get().SetBackCull(false);

                for (auto meshIt = m_currentScene->begin<MeshComponent>(); meshIt != m_currentScene->end<MeshComponent>(); meshIt++)
                {
                    MeshComponent* meshComponent = reinterpret_cast<MeshComponent*>(meshIt->second);
                    if (meshComponent && meshComponent->IsActive() && EntityManager::Get().GetEntity(meshComponent->GetOwnerID())->IsActive())
                    {
                        const Resources::SubMeshPtr mesh = meshComponent->GetMesh();
                        if (mesh && mesh->GetState() == AResource::E_ResourceState::LOADED)
                        {
                            Maths::Matrix4& meshTransform = SceneSystem::GetWorldTransform(meshComponent->GetOwnerID());
                            shadowMtl->SetUniform("uModel", meshTransform);

                            Graphics::IRenderer::Get().Render(*mesh, shadowMtl, m_useWireframe);
                            m_drawCallCount++;
                        }
                    }
                }
				ComponentManager&   componentManager = ComponentManager::Get();
				for (auto animIt = m_currentScene->begin<AnimationComponent>(); animIt != m_currentScene->end<AnimationComponent>(); animIt++)
				{
					AnimationComponent* SkelMeshComponent = reinterpret_cast<AnimationComponent*>(animIt->second);
					if (SkelMeshComponent->GetSkeletalMesh() != nullptr)
					{
						if (SkelMeshComponent->IsActive()
							&& EntityManager::Get().GetEntity(SkelMeshComponent->GetOwnerID())->IsActive()
							&& SkelMeshComponent->GetSkeletalMesh()->GetState() == AResource::E_ResourceState::LOADED)
						{
							Maths::Matrix4&     meshTransform = SceneSystem::GetWorldTransform(SkelMeshComponent->GetOwnerID());
							for (Resources::AnimMeshPtr& pMesh : SkelMeshComponent->GetSkeletalMesh()->GetAnimationMesh())
							{
								MaterialComponent*  materialComponent = componentManager.GetComponent<MaterialComponent>(pMesh->GetOwnerID());
								if (materialComponent && materialComponent->IsActive() && materialComponent->GetMaterial())
								{
									shadowMtl->SetUniform("uModel", meshTransform);
									
									Graphics::IRenderer::Get().Render(*pMesh, shadowMtl, m_useWireframe);
									++m_drawCallCount;
								}
							}
						}
					}
				}
            }
        }

        Graphics::IRenderer::Get().ShadowBuffer->Unbind();
        Graphics::IRenderer::Get().ShadowBuffer->BindTexture();

        Graphics::IRenderer::Get().SetFrontCull(false);
        Graphics::IRenderer::Get().SetBackCull(true);
    }

    void RenderSystem::GeometryPass()
    {
        Graphics::IRenderer::Get().GBuffer->Bind();
		Graphics::IRenderer::Get().GBuffer->BindTextures();
        Graphics::IRenderer::Get().Clear();
		Graphics::IRenderer::Get().SetBlendTest(false);
        Graphics::IRenderer::Get().SetDepthTest(true);

        ComponentManager&   componentManager    = ComponentManager::Get();

        for (auto meshIt = m_currentScene->begin<MeshComponent>(); meshIt != m_currentScene->end<MeshComponent>(); meshIt++)
        {
            MeshComponent* meshComponent = reinterpret_cast<MeshComponent*>(meshIt->second);
            if (meshComponent && meshComponent->IsActive() && EntityManager::Get().GetEntity(meshComponent->GetOwnerID())->IsActive())
            {
                const Resources::SubMeshPtr mesh = meshComponent->GetMesh();
                if (mesh && mesh->GetState() == AResource::E_ResourceState::LOADED)
                {
                    Maths::Matrix4&     meshTransform       = SceneSystem::GetWorldTransform(meshComponent->GetOwnerID());
                    MaterialComponent*  materialComponent   = componentManager.GetComponent<MaterialComponent>(meshComponent->GetOwnerID());
                    if (materialComponent && materialComponent->IsActive() && materialComponent->GetMaterial())
                    {
                        materialComponent->GetMaterial()->SetUniform("uModel", meshTransform);
						Graphics::IRenderer::Get().Render(*mesh, materialComponent->GetMaterial(), m_useWireframe);
                        ++m_drawCallCount;
                    }
                }
            }
        }

		for (auto animIt = m_currentScene->begin<AnimationComponent>(); animIt != m_currentScene->end<AnimationComponent>(); animIt++)
		{
			AnimationComponent* SkelMeshComponent = reinterpret_cast<AnimationComponent*>(animIt->second);
			if (SkelMeshComponent->GetSkeletalMesh() != nullptr)
			{
				if (SkelMeshComponent->IsActive()
					&& EntityManager::Get().GetEntity(SkelMeshComponent->GetOwnerID())->IsActive()
					&& SkelMeshComponent->GetSkeletalMesh()->GetState() == AResource::E_ResourceState::LOADED)
				{
					Maths::Matrix4&     meshTransform = SceneSystem::GetWorldTransform(SkelMeshComponent->GetOwnerID());
					for (Resources::AnimMeshPtr& pMesh : SkelMeshComponent->GetSkeletalMesh()->GetAnimationMesh())
					{
						MaterialComponent*  materialComponent = componentManager.GetComponent<MaterialComponent>(pMesh->GetOwnerID());
						if (materialComponent && materialComponent->IsActive() && materialComponent->GetMaterial())
						{
							materialComponent->GetMaterial()->SetUniform("uModel", meshTransform);
							Graphics::IRenderer::Get().Render(*pMesh, materialComponent->GetMaterial(), m_useWireframe);
							++m_drawCallCount;
						}
					}
				}
			}
		}

        auto& skybox = m_currentScene->GetSkybox();
        const SceneRenderParam& renderParams = m_currentScene->GetRenderParams();
        if (skybox)
        {
            Resources::MaterialPtr cubeMapConvertMtl = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("Skybox", "RenderGroup"_sid).first);
            cubeMapConvertMtl->SetShader(ShaderManager::Get().GetShader("Skybox"_sid, "DefaultResources"_sid));
            cubeMapConvertMtl->SetTexture("uEnvironmentMap",    skybox);
            cubeMapConvertMtl->SetUniform("uExposure",          renderParams.skyboxExposure);
            cubeMapConvertMtl->SetUniform("uGamma",             renderParams.gamma);
            cubeMapConvertMtl->Bind();
            skybox->Render();
            m_drawCallCount++;
        }


        //Grid
        if (!m_systemManager->IsInPlayMode())
        {
			Graphics::IRenderer::Get().SetBlendTest(true);
            Graphics::IRenderer::Get().SetBackCull(false);
            auto gridMesh = MeshManager::Get().GetMesh("Plane"_sid, "DefaultResources"_sid);
            if (!gridMesh || gridMesh->GetState() != AResource::E_ResourceState::LOADED)
                return;
            Graphics::IRenderer::Get().Render(*gridMesh->GetSubMeshes()[0], MaterialManager::Get().GetMaterial("GridMaterial"_sid, "Editor"_sid), false);
            Graphics::IRenderer::Get().SetBackCull(true);
            m_drawCallCount++;
        }

		Graphics::IRenderer::Get().GBuffer->Unbind();
    }

    void RenderSystem::BeautyPass()
    {
        Graphics::IRenderer::Get().RenderTexture->Bind();
		Graphics::IRenderer::Get().RenderTexture->BindTextures();

        Graphics::IRenderer::Get().Clear(Graphics::COLOR_BIT);
        Graphics::IRenderer::Get().SetDepthTest(false);


        if (!m_quadMaterial)
        {
            m_quadMaterial = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("QuadMaterial", "RenderGroup"_sid).first);
        }

        m_quadMaterial->SetShader(ShaderManager::Get().GetShader("PBR_lighting"_sid));

        auto& skybox = m_currentScene->GetSkybox();
        m_quadMaterial->SetTexture("uIrradiance",          skybox, E_ShaderType::SHADER_TYPE_SAMPLERCUBE_IRR);
        m_quadMaterial->SetTexture("uPrefilterMap",        skybox, E_ShaderType::SHADER_TYPE_SAMPLERCUBE_PREFILTER);
        m_quadMaterial->SetTexture("uBRDFTable",           TextureManager::Get().GetTexture("BRDF"_sid, "RenderGroup"_sid));
        
        const SceneRenderParam& renderParams = m_currentScene->GetRenderParams();

        m_quadMaterial->SetUniform("uExposure",            renderParams.skyboxExposure);
        m_quadMaterial->SetUniform("uThreshold",           renderParams.bloomThreshold);						// Brightness threshold
        m_quadMaterial->SetUniform("uGamma",               renderParams.gamma);									// Gamma

        m_quadMaterial->SetTexture("gPosition",            TextureManager::Get().GetTexture("GPosition"_sid, "RenderGroup"_sid));
        m_quadMaterial->SetTexture("gNormal",              TextureManager::Get().GetTexture("GNormal"_sid, "RenderGroup"_sid));
        m_quadMaterial->SetTexture("gAlbedo",              TextureManager::Get().GetTexture("GAlbedo"_sid, "RenderGroup"_sid));
        m_quadMaterial->SetTexture("gMetallicRoughness",   TextureManager::Get().GetTexture("GMetallicRoughness"_sid, "RenderGroup"_sid));
        m_quadMaterial->SetTexture("gEmissive",            TextureManager::Get().GetTexture("GEmissive"_sid, "RenderGroup"_sid));
        m_quadMaterial->SetTexture("gUnlit",               TextureManager::Get().GetTexture("GUnlit"_sid, "RenderGroup"_sid));


        if (m_shadowsLightSpace.size() > 0)
        {
            m_quadMaterial->SetUniform("uShadowsEnabled", true);
            m_quadMaterial->SetUniform("uLightSpaceMatrix", m_shadowsLightSpace[0]);
        }
        else
        {
            m_quadMaterial->SetUniform("uShadowsEnabled", false);
            m_quadMaterial->SetUniform("uLightSpaceMatrix", Maths::Matrix4());
        }
        m_quadMaterial->SetTexture("uShadow", TextureManager::Get().GetTexture("ShadowTexture"_sid, "RenderGroup"_sid));

        Graphics::IRenderer::Get().Render(*m_quadRender.GetSubMeshes()[0], m_quadMaterial, false);
        m_drawCallCount++;

        Graphics::IRenderer::Get().RenderTexture->Unbind();
    }

	void RenderSystem::BloomPass()
	{
		
		Graphics::IRenderer::Get().RenderTexture->Bind();
		Graphics::IRenderer::Get().RenderTexture->BindTextures();
		m_quadMaterial = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("BloomMaterial", "RenderGroup"_sid).first);
		m_quadMaterial->SetShader(ShaderManager::Get().GetShader("bloom"_sid));
		m_quadMaterial->SetTexture("uImage",       Graphics::IRenderer::Get().PongBuffer->GetTexture());
		m_quadMaterial->SetTexture("uBloomBlur",   Graphics::IRenderer::Get().PingBuffer->GetTexture());
		m_quadMaterial->SetUniform("uIntensity",   m_currentScene->GetRenderParams().bloomIntesity);
        
		Graphics::IRenderer::Get().Clear();
		Graphics::IRenderer::Get().Render(*m_quadRender.GetSubMeshes()[0], m_quadMaterial, false);
		Graphics::IRenderer::Get().RenderTexture->Unbind();
	}

	void RenderSystem::PingPongRender(Graphics::Buffers::IFrameBuffer* p_src, Graphics::Buffers::IFrameBuffer* p_dest, Resources::MaterialPtr p_mat)
	{
		p_dest->Bind();
		p_dest->BindTexture();

		p_mat->SetTexture("uImage", p_src->GetTexture());

		Graphics::IRenderer::Get().Render(*m_quadRender.GetSubMeshes()[0], p_mat, false);

		p_dest->Unbind();
		Graphics::IRenderer::Get().Blit(p_dest, p_src);
		p_src->Unbind();

	}

	void RenderSystem::BlurImage(int p_steps, Resources::TexturePtr p_texture)
	{
		Resources::MaterialPtr copy = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("CopyMaterial", "RenderGroup"_sid).first);
		copy->SetShader(ShaderManager::Get().GetShader("copy"_sid));
		copy->SetTexture("uImage", p_texture);

		Graphics::IRenderer::Get().PingBuffer->Bind();
		Graphics::IRenderer::Get().PingBuffer->BindTexture();
		Graphics::IRenderer::Get().Clear();

		Graphics::IRenderer::Get().Render(*m_quadRender.GetSubMeshes()[0], copy, false);
		Graphics::IRenderer::Get().PingBuffer->Unbind();
		m_quadMaterial = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("BloomMaterial", "RenderGroup"_sid).first);
		m_quadMaterial->SetShader(ShaderManager::Get().GetShader("gaussianBlur"_sid));
		bool horizontal = true;
		
		for (int i = 0; i < p_steps; ++i)
		{
			m_quadMaterial->SetUniform("horizontal", horizontal);
			PingPongRender(Graphics::IRenderer::Get().PingBuffer, Graphics::IRenderer::Get().PongBuffer, m_quadMaterial);
			horizontal = !horizontal;
		}
	}

	unsigned RenderSystem::GetDrawCallCount() const
    {
        return m_drawCallCount;
    }
}