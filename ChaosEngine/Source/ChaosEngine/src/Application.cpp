#include "ChaosEngine/Application.h"
#include "ChaosEngine/Base.h"
#include "ChaosEngine/Loader.h"
#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Context/GLFWInput.h"
#include <random>

namespace Chaos
{
  
    Application::Application()
    {
        m_window = std::unique_ptr<Graphics::IWindow>(Graphics::IWindow::Create());
        Graphics::IRenderer::Init();

        ResourceGroupManager::Get().LoadResourceDeclaration("Assets/DefaultResources/Defaults.resdecl");
        ResourceGroupManager::Get().LoadResourceDeclaration("Assets/DefaultResources/Editor.resdecl");

        ResourceGroupManager::Get().InitializeAllResourceGroups();
        ResourceGroupManager::Get().LoadResourceGroup("DefaultResources"_sid);
        ResourceGroupManager::Get().LoadResourceGroup("DefaultSkies"_sid);
        ResourceGroupManager::Get().LoadResourceGroup("Editor"_sid);
        ScenePtr scene = std::reinterpret_pointer_cast<Scene>(SceneManager::Get().CreateOrRetrieve("Animation", "DefaultResources"_sid).first);

		
        auto& mainCamera = EntityManager::Get().CreateEntity<CameraGameObject>("MainCamera"_sid);
        mainCamera->GetComponent<TransformComponent>()->AsTransform().SetPosition(-40.f, 10.f, 0.f);
        PhysicsSystem*  physSys     = SystemManager::Get().AddSystem<PhysicsSystem>();
        RenderSystem*   renderSys   = SystemManager::Get().AddSystem<RenderSystem>();
        SceneSystem*    sceneSys    = SystemManager::Get().AddSystem<SceneSystem>(scene);
        CameraSystem*   cameraSys   = SystemManager::Get().AddSystem<CameraSystem>(mainCamera->GetEntityID());
        LightSystem*    lightSys    = SystemManager::Get().AddSystem<LightSystem>();
		AnimationSystem* animSys    = SystemManager::Get().AddSystem<AnimationSystem>();
		
        ScriptsHandler::Get().CompileComponents();
        ScriptsHandler::Get().CompileSystems();
		mainCamera->AddScriptedComponent("FPControllerComponent");
        SystemManager::Get().UpdateSystemWorkOrder();
		scene->AddEntity(mainCamera);

		scene->AddSystem(physSys);
		scene->AddSystem(renderSys);
        scene->AddSystem(sceneSys);
        scene->AddSystem(cameraSys);
        scene->AddSystem(lightSys);
		scene->AddSystem(animSys);


        {
            RegisterEventCallback<Events::KeyPressedEvent>(std::bind(&Application::OnKeyPressed, this, std::placeholders::_1));
            RegisterEventCallback<Events::KeyReleasedEvent>(std::bind(&Application::OnKeyReleased, this, std::placeholders::_1));
            RegisterEventCallback<Events::WindowsCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
        }


        scene->SetSkybox(SkyboxManager::Get().GetCubeMap("Newport_Loft"_sid, "DefaultSkies"_sid));
    }

    bool Application::OnKeyReleased(const Events::KeyReleasedEvent& e)
    {
        switch (e.KeyCode())
        {
        case E_ChaosKeyCode::CHAOS_KEY_ESCAPE:
            m_running = false;
            break;
        case E_ChaosKeyCode::CHAOS_KEY_R:
            if (Input::Get().IsKeyPressed(E_ChaosKeyCode::CHAOS_KEY_LEFT_CONTROL))
            {
                ScriptsHandler::Get().CompileComponents();
                ScriptsHandler::Get().CompileSystems();
            }
            else
                ShaderManager::Get().ReloadAll();
            break;
        case E_ChaosKeyCode::CHAOS_KEY_L:
            break;
        case E_ChaosKeyCode::CHAOS_KEY_F2:
        {
            
            CHAOS_CORE_FATAL("FPS: {} | DrawCalls: {}", 1.0f / Utils::Timer::DeltaTime().count(), SystemManager::Get().GetSystem<RenderSystem>()->GetDrawCallCount());
            break;
        }
        case E_ChaosKeyCode::CHAOS_KEY_F3:
        {
            SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene()->DebugPrint();
            break;
        }
        case E_ChaosKeyCode::CHAOS_KEY_F9:
        {
            SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene()->Save();
            break;
        }
        case E_ChaosKeyCode::CHAOS_KEY_F10:
        {
            SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene()->Load();
            break;
        }
        default:
            break;
        }
        return false;
    }

    bool Application::OnKeyPressed(const Events::KeyPressedEvent& e)
    {
        return false;
    }

    bool Application::OnWindowClose(const Events::WindowsCloseEvent& e)
    {
        m_running = false;
        return true;
    }

    void Application::Run()
    {

        using Clock = std::chrono::high_resolution_clock;
        Resources::TexturePtr   gunAlbedo		= TextureManager::Get().GetTexture("Revolver_AAO_s"_sid, "Globals"_sid);
        Resources::TexturePtr   gunNormal		= TextureManager::Get().GetTexture("Revolver_N_s"_sid, "Globals"_sid);
        Resources::TexturePtr   gunMetallic		= TextureManager::Get().GetTexture("Revolver_M_s"_sid, "Globals"_sid);
        Resources::TexturePtr   gunRoughness	= TextureManager::Get().GetTexture("Revolver_R_s"_sid, "Globals"_sid);
        Resources::TexturePtr   gunEmissive		= TextureManager::Get().GetTexture("Revolver_E_s"_sid, "Globals"_sid);
		Resources::SkeletalMeshPtr TestAnim = AnimationManager::Get().GetSkeletalMesh("WaveDance"_sid, "Editor"_sid);
		Resources::SkeletalMeshPtr TestAnim2 = AnimationManager::Get().GetSkeletalMesh("Around"_sid, "Editor"_sid);
		Resources::MaterialPtr	AnimMat = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("RevolverMaterial", "Editor"_sid).first);
		AnimMat->SetTexture("uTexture",      gunAlbedo);
		AnimMat->SetTexture("uNormal",       gunNormal);
		AnimMat->SetTexture("uMetallic",     gunMetallic);
		AnimMat->SetTexture("uRoughness",    gunRoughness);
		AnimMat->SetTexture("uEmissive",     gunEmissive);

            
		SkeletalAnimationGameObject& animation = SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<SkeletalAnimationGameObject>("Animation1"_sid, TestAnim, AnimMat);
		Chaos::Animation::SkinningMode mode = Chaos::Animation::SkinningMode::DUALQUAT;
		animation.GetComponent<AnimationComponent>()->GetAnimator()->SetSkinningMode(mode);
		animation.GetComponent<AnimationComponent>()->PlayAnimation();
		animation.GetComponent<TransformComponent>()->AsTransform().SetPosition(0.f, 0, 20.f);
		Maths::Quaternion rotated; 
		rotated.MakeFromEulerAngles(Maths::Vector3(0, -90, 0), 1, 2, 3);
		animation.GetComponent<TransformComponent>()->AsTransform().SetOrientation(rotated);
		animation.GetComponent<TransformComponent>()->AsTransform().SetScale(0.1f, 0.1f, 0.1f);

        Loader::Get().GetSystemManager().Start();
        CHAOS_CORE_TRACE("Play Mode: {}", Loader::Get().GetSystemManager().IsInPlayMode());
        while (m_running)
        {

			
            Utils::Timer::Frame();

            // FixedUpdate
            while (Utils::Timer::Accumulator() >= Utils::Timer::FIXED_DELTA_TIME)
            {
                Utils::Timer::FixedUpdate();
                Chaos::FixedUpdate(Utils::Timer::FIXED_DELTA_TIME.count());
            }

            Chaos::Update(Utils::Timer::DeltaTime().count());
			m_window->OnUpdate();
        }
    }
}
