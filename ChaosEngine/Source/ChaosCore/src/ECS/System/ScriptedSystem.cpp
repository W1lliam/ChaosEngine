#include "ChaosCore/ECS/System/ScriptedSystem.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"
#include <ChaosDebug.h>

namespace Chaos
{
    CHAOS_DEFINE_SYSTEM_TYPEID(ScriptedSystem);

    ScriptedSystem::ScriptedSystem(ssq::VM& p_vm) : System(LOWEST_SYSTEM_PRIORITY),
        m_vm(p_vm),
        m_sqInstance(p_vm.find("instance").toInstance())
    {
        m_typeName = m_sqInstance.find("typeName").toString();
        m_typeID = operator""_sid(m_typeName.c_str(), m_typeName.size());

        m_startFunc         = FetchFunction("Start");
        m_stopFunc          = FetchFunction("Stop");

        m_fixedUpdateFunc   = FetchFunction("FixedUpdate");
        m_preUpdateFunc     = FetchFunction("PreUpdate");
        m_updateFunc        = FetchFunction("Update");
        m_postUpdateFunc    = FetchFunction("PostUpdate");

        m_onKeyPressedFunc  = FetchFunction("OnKeyPressed");
        if(m_onKeyPressedFunc)
            RegisterEventCallback<Events::KeyPressedEvent>(std::bind(&ScriptedSystem::OnKeyPressed, this, std::placeholders::_1));

        m_onKeyReleasedFunc = FetchFunction("OnKeyReleased");
        if (m_onKeyReleasedFunc)
            RegisterEventCallback<Events::KeyReleasedEvent>(std::bind(&ScriptedSystem::OnKeyReleased, this, std::placeholders::_1));


    }

    ScriptedSystem::~ScriptedSystem()
    {
        if (m_startFunc)
            delete m_startFunc;
        if (m_stopFunc)
            delete m_stopFunc;
        if (m_fixedUpdateFunc)
            delete m_fixedUpdateFunc;
        if (m_preUpdateFunc)
            delete m_preUpdateFunc;
        if (m_updateFunc)
            delete m_updateFunc;
        if (m_postUpdateFunc)
            delete m_postUpdateFunc;

        if (m_onKeyPressedFunc)
            delete m_onKeyPressedFunc;
        if (m_onKeyReleasedFunc)
            delete m_onKeyReleasedFunc;
    }

    void ScriptedSystem::Start()
    {
        if (m_startFunc)
        {
            try
            {
                m_vm.callFunc(*m_startFunc, m_sqInstance);
            }
            catch (ssq::RuntimeException p_exception)
            {
				CHAOS_CORE_WARN("Script execution failed: Start() {}", p_exception.what());
            }
        }
    }

    void ScriptedSystem::Stop()
    {
        if (m_stopFunc)
        {
            try
            {
                m_vm.callFunc(*m_stopFunc, m_sqInstance);
            }
            catch (ssq::RuntimeException p_exception)
            {
				CHAOS_CORE_WARN("Script execution failed: Stop() {}", p_exception.what());
            }
        }
    }

    void ScriptedSystem::FixedUpdate(double p_fixedDeltaTime)
    {
        if (m_fixedUpdateFunc)
        {
            try 
            {
                m_vm.callFunc(*m_fixedUpdateFunc, m_sqInstance, (float)p_fixedDeltaTime);
            }
            catch (ssq::RuntimeException p_exception)
            {
				CHAOS_CORE_WARN("Script execution failed: FixedUpdate() {}", p_exception.what());
            }
        }
    }

    void ScriptedSystem::PreUpdate(double p_dt)
    {
        if (m_preUpdateFunc)
        {
            try
            {
                m_vm.callFunc(*m_preUpdateFunc, m_sqInstance, (float)p_dt);
            }
            catch (ssq::RuntimeException p_exception)
            {
                CHAOS_CORE_WARN("Script execution failed: PreUpdate() {}", p_exception.what());
            }
        }
    }

    void ScriptedSystem::Update(double p_dt)
    {
        if (m_updateFunc)
        {
            try
            {
                m_vm.callFunc(*m_updateFunc, m_sqInstance, (float)p_dt);
            }
            catch (ssq::RuntimeException p_exception)
            {
				CHAOS_CORE_WARN("Script execution failed: Update() {}", p_exception.what());
            }
        }
    }

    void ScriptedSystem::PostUpdate(double p_dt)
    {
        if (m_postUpdateFunc)
        {
            try
            {
                m_vm.callFunc(*m_postUpdateFunc, m_sqInstance, (float)p_dt);
            }
            catch (ssq::RuntimeException p_exception)
            {
				CHAOS_CORE_WARN("Script execution failed: PostUpdate() {}", p_exception.what());
            }
        }
    }
    bool ScriptedSystem::OnKeyPressed(const Events::KeyPressedEvent& p_event)
    {
        if (m_onKeyPressedFunc && m_enabled && 
            ((m_systemManager->IsInPlayMode() && m_executionMode & E_SystemExecutionModeFlags::GAME) ||
            (!m_systemManager->IsInPlayMode() && m_executionMode & E_SystemExecutionModeFlags::EDITOR)))
        {
            try
            {
                m_vm.callFunc(*m_onKeyPressedFunc, m_sqInstance, (int)p_event.KeyCode(), p_event.GetRepeatCount());
            }
            catch (ssq::RuntimeException p_exception)
            {
				CHAOS_CORE_WARN("Script execution failed: OnKeyPressed() {}", p_exception.what());
            }
        }
        return false;
    }
    bool ScriptedSystem::OnKeyReleased(const Events::KeyReleasedEvent & p_event)
    {
        if (m_onKeyReleasedFunc && m_enabled &&
            ((m_systemManager->IsInPlayMode() && m_executionMode & E_SystemExecutionModeFlags::GAME) ||
            (!m_systemManager->IsInPlayMode() && m_executionMode & E_SystemExecutionModeFlags::EDITOR)))
        {
            try
            {
                m_vm.callFunc(*m_onKeyReleasedFunc, m_sqInstance, (int)p_event.KeyCode());
            }
            catch (ssq::RuntimeException p_exception)
            {
				CHAOS_CORE_WARN("Script execution failed: OnKeyReleased() {}", p_exception.what());
            }
        }
        return false;
    }

    bool ScriptedSystem::Save(WriteStream& p_stream)
    {
        ssq::Function* saveFunc = FetchFunction("Save");
        if (saveFunc)
        {
            try
            {
                m_vm.callFunc(*saveFunc, m_sqInstance, p_stream);
            }
            catch (ssq::RuntimeException p_exception)
            {
				CHAOS_CORE_WARN("Script execution failed: OnKeyReleased() {}", p_exception.what());
            }
        }
        System::Save(p_stream);
        return true;
    }

    System& ScriptedSystem::Load(ReadStream& p_stream, SystemTypeID p_systemTypeID, const std::string& p_systemName)
    {
        ScriptedSystem* system = ScriptsHandler::Get().CompileSystem(p_systemName);
        ssq::Function* loadFunc = system->FetchFunction("Load");
        if (loadFunc)
        {
            try
            {
                system->m_vm.callFunc(*loadFunc, system->m_sqInstance, p_stream);
            }
            catch (ssq::RuntimeException p_exception)
            {
				CHAOS_CORE_WARN("Script execution failed: OnKeyReleased() {}", p_exception.what());
            }
        }
        static_cast<System*>(system)->Load(p_stream);
        return *system;
    }
    ssq::Function* ScriptedSystem::FetchFunction(const std::string& p_functionName)
    {
        CHAOS_CORE_ASSERT(!m_sqInstance.isEmpty(), "Uninitialized instance");
        try
        {
            return new ssq::Function(m_sqInstance.find(p_functionName.c_str()));
        }
        catch (ssq::NotFoundException)
        {
            return nullptr;
        }
    }
}