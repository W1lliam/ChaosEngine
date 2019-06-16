#include "ChaosEngine/Loader.h"
#include <ChaosDebug.h>

namespace Chaos
{

    Loader* Loader::s_instance  {nullptr};

    Loader& Loader::Get()
    {
        assert(s_instance && "Loader not initialized!");
        return *s_instance;
    }

    Loader* Loader::GetPointer()
    {
        return s_instance;
    }

    Loader::Loader()
    {
        CHAOS_CORE_ASSERT(!s_instance, "Loader already initialized!");
        s_instance = this;
        if (!m_logManager)
            m_logManager = new Debug::Log();

        if (!ScriptsHandler::GetPointer())
            m_scriptsHandler = new ScriptsHandler();

        if (!m_elementCreator)
            m_elementCreator = new Physics::ElementCreator();

        if (!Events::EventHandler::GetPointer())
            m_eventHandler = new Events::EventHandler();

        if (!Graphics::IRenderer::GetPointer())
			m_renderer = Graphics::IRenderer::Create();


        if(!ResourceGroupManager::GetPointer())
            m_resourceGroupManager = new ResourceGroupManager();

        if (!SceneManager::GetPointer())
            m_sceneManager = new SceneManager();

        if (!MeshManager::GetPointer())
            m_meshManager = new MeshManager();

        if (!ShaderManager::GetPointer())
            m_shaderManager = new ShaderManager();

        if (!TextureManager::GetPointer())
            m_textureManager = new TextureManager();     

        if (!SkyboxManager::GetPointer())
            m_skyboxManager = new SkyboxManager();
		
		if (!MaterialManager::GetPointer())
			m_materialManager = new MaterialManager();
		
		if (!AudioResourcesManager::GetPointer())
			m_audioResourceManager = new AudioResourcesManager();

		if (!AnimationManager::GetPointer())
			m_animationManager = new AnimationManager();
        
        if (!EntityManager::GetPointer())
            m_entityManager = new EntityManager();

        if (!ComponentManager::GetPointer())
            m_componentManager = new ComponentManager();
        
        if (!SystemManager::GetPointer())
            m_systemManager = new SystemManager();
    }
    Loader::~Loader()
    {
        CHAOS_CORE_ASSERT(s_instance, "Loader not initialized!");

        delete m_systemManager;
        delete m_componentManager;
        delete m_entityManager;

		delete m_audioResourceManager;
        delete m_materialManager;
        delete m_textureManager;
        delete m_shaderManager;
        delete m_meshManager;
        delete m_sceneManager;
        delete m_skyboxManager;
		delete m_animationManager;

        delete m_resourceGroupManager;

        delete m_renderer;
        delete m_eventHandler;
        delete m_elementCreator;
        delete m_scriptsHandler;
        delete m_input;
        delete m_logManager;
        s_instance = nullptr;
    }
}

extern "C"
{
    namespace Chaos
    {
        void SetInput(Input* p_inputInstance)
        {
            Loader::Get().SetInput(p_inputInstance);
        }

        Debug::Log& GetLogger()
        {
            return Loader::Get().GetLogger();
        }

        Input& GetInput()
        {
            return Loader::Get().GetInput();
        }

        ScriptsHandler& GetScriptsHandler()
        {
            return Loader::Get().GetScriptsHandler();
        }

        Physics::ElementCreator& GetElementCreator()
        {
            return Loader::Get().GetElementCreator();
        }
        Events::EventHandler& GetEventHandler()
        {
            return Loader::Get().GetEventHandler();
        }

        Graphics::IRenderer & GetIRenderer()
		{
			return Loader::Get().GetIRenderer();
		}

        ResourceGroupManager& GetResourceGroupManager()
        {
            return Loader::Get().GetResourceGroupManager();
        }
        SceneManager& GetSceneManager()
        {
            return Loader::Get().GetSceneManager();
        }
        MeshManager& GetMeshManager()
        {
            return Loader::Get().GetMeshManager();
        }
        ShaderManager& GetShaderManager()
        {
            return Loader::Get().GetShaderManager();
        }
        TextureManager& GetTextureManager()
        {
            return Loader::Get().GetTextureManager();
        }
        SkyboxManager& GetSkyboxManager()
        {
            return Loader::Get().GetSkyboxManager();
        }
		MaterialManager& GetMaterialManager()
        {
            return Loader::Get().GetMaterialManager();
        }

		AudioResourcesManager& GetAudioResourcesManager()
		{
			return Loader::Get().GetAudioResourcesManager();
		}

		AnimationManager & GetAnimationManager()
		{
			return Loader::Get().GetAnimationManager();
		}

        EntityManager& GetEntityManager()
        {
            return Loader::Get().GetEntityManager();
        }
        ComponentManager& GetComponentManager()
        {
            return Loader::Get().GetComponentManager();
        }
       SystemManager& GetSystemManager()
        {
            return Loader::Get().GetSystemManager();
        }
    }
}