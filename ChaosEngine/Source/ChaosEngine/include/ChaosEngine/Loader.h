#pragma once
#include "ChaosEngine/export.h"
#include <ChaosCore.h>

namespace Chaos
{
    class Input;

    class CHAOS_API Loader
    {
    public:
        static Loader& Get();
        static Loader* GetPointer();

        inline Debug::Log&              GetLogger()                 { return *m_logManager; };
        inline Input&                   GetInput()                  { return *m_input; };
        inline ScriptsHandler&          GetScriptsHandler()         { return *m_scriptsHandler; };
        inline Physics::ElementCreator& GetElementCreator()         { return *m_elementCreator; };
        inline Events::EventHandler&    GetEventHandler()           { return *m_eventHandler; };

        inline Graphics::IRenderer&     GetIRenderer()              { return *m_renderer; };

        inline ResourceGroupManager&    GetResourceGroupManager()   { return *m_resourceGroupManager; };
        inline SceneManager&            GetSceneManager()           { return *m_sceneManager; };
        inline MeshManager&             GetMeshManager()            { return *m_meshManager; };
        inline ShaderManager&           GetShaderManager()          { return *m_shaderManager; };
        inline TextureManager&          GetTextureManager()         { return *m_textureManager; };
        inline SkyboxManager&           GetSkyboxManager()          { return *m_skyboxManager; };
        inline MaterialManager&         GetMaterialManager()        { return *m_materialManager; };
		inline AudioResourcesManager&   GetAudioResourcesManager()	{ return *m_audioResourceManager; };
		inline AnimationManager&		GetAnimationManager()		{ return *m_animationManager; };

        inline EntityManager&           GetEntityManager()          { return *m_entityManager; };
        inline ComponentManager&        GetComponentManager()       { return *m_componentManager; };
        inline SystemManager&           GetSystemManager()          { return *m_systemManager; };

        inline void                     SetInput(Input* p_inputInstance) { m_input = p_inputInstance; };

        Loader();
        ~Loader();

        Loader(const Loader&) = delete;
        void operator=(const Loader&) = delete;
    protected:
        static Loader* s_instance;

        Debug::Log*                 m_logManager            {nullptr};
        Input*                      m_input                 {nullptr};
        ScriptsHandler*             m_scriptsHandler        {nullptr};
        Physics::ElementCreator*    m_elementCreator        {nullptr};
        Events::EventHandler*       m_eventHandler          {nullptr};

		Graphics::IRenderer*	    m_renderer				{nullptr};

        ResourceGroupManager*       m_resourceGroupManager  {nullptr};
        SceneManager*               m_sceneManager          {nullptr};
		MaterialManager*            m_materialManager		{nullptr};
        MeshManager*                m_meshManager           {nullptr};
        ShaderManager*              m_shaderManager         {nullptr};
        TextureManager*             m_textureManager        {nullptr};
        SkyboxManager*              m_skyboxManager         {nullptr};

        EntityManager*              m_entityManager         {nullptr};
        ComponentManager*           m_componentManager      {nullptr};
        SystemManager*              m_systemManager         {nullptr};
		AudioResourcesManager*		m_audioResourceManager	{nullptr};

		AnimationManager*			m_animationManager		{ nullptr };
    };

}

extern "C"
{
    namespace Chaos
    {
        CHAOS_API void                      SetInput(Input* p_inputInstance);

        CHAOS_API Debug::Log&               GetLogger();
        CHAOS_API Input&                    GetInput();
        CHAOS_API ScriptsHandler&           GetScriptsHandler();
        CHAOS_API Physics::ElementCreator&  GetElementCreator();
        CHAOS_API Events::EventHandler&     GetEventHandler();

        CHAOS_API Graphics::IRenderer&      GetIRenderer();
        
        CHAOS_API ResourceGroupManager&     GetResourceGroupManager();
        CHAOS_API SceneManager&             GetSceneManager();
        CHAOS_API MeshManager&              GetMeshManager();
        CHAOS_API ShaderManager&            GetShaderManager();
        CHAOS_API TextureManager&           GetTextureManager();
        CHAOS_API SkyboxManager&            GetSkyboxManager();
        CHAOS_API MaterialManager&          GetMaterialManager();
        CHAOS_API AudioResourcesManager&    GetAudioResourcesManager();
		CHAOS_API AnimationManager&			GetAnimationManager();

        CHAOS_API EntityManager&            GetEntityManager();
        CHAOS_API ComponentManager&         GetComponentManager();
        CHAOS_API SystemManager&            GetSystemManager();
    }
}