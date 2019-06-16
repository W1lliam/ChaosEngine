#include "ChaosCore/ECS/Component/ScriptedComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(ScriptedComponent);

    ScriptedComponent::ScriptedComponent(COMPONENT_INIT_PARAMS, const std::string& p_typeName) :
        COMPONENT_INIT(ScriptedComponent),
        m_vm(ScriptsHandler::Get().GetVirtualMachine()),
        m_typeName(p_typeName)
    {
        m_typeID = operator""_sid(m_typeName.c_str(), m_typeName.size());
    }

    ScriptedComponent::~ScriptedComponent()
    {
        if (m_saveFunc)
            delete m_saveFunc;
        if (m_loadFunc)
            delete m_loadFunc;
    }

    bool ScriptedComponent::Save(WriteStream & p_stream)
    {
        SerializeString(p_stream, m_typeName);
        return true;
    }

    IComponent& ScriptedComponent::Load(ReadStream& p_stream, const EntityID& p_entityID)
    {
        std::string typeName;
        SerializeString(p_stream, typeName);
        return ComponentManager::Get().AddScriptedComponent(p_entityID, typeName);
    }

    void ScriptedComponent::Expose()
    {
        ssq::Class sqComp = ScriptsHandler::Get().GetVirtualMachine().addClass("ScriptedComponent", ssq::Class::Ctor<ScriptedComponent(EntityID, ComponentID, std::string)>());
    }
}