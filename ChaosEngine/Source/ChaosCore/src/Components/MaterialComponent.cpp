#include "ChaosCore/Components/MaterialComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/Resources/Managers/MaterialManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(MaterialComponent);

	MaterialComponent::MaterialComponent(COMPONENT_INIT_PARAMS, Resources::MaterialPtr p_material) : 
		COMPONENT_INIT(MaterialComponent), 
		m_material(p_material)
	{
	}

    bool MaterialComponent::Save(WriteStream & p_stream)
    {
        bool hasMaterial = m_material != nullptr;
        SerializeBool(p_stream, hasMaterial);
        if (hasMaterial)
        {
            SerializeStringID(p_stream, const_cast<StringID&>(m_material->GetGroup()));
            SerializeStringID(p_stream, const_cast<StringID&>(m_material->GetName()));
        }
        return true;
    }

    IComponent& MaterialComponent::Load(ReadStream & p_stream, const EntityID & p_entityID)
    {
        bool hasMaterial;
        SerializeBool(p_stream, hasMaterial);
        if (hasMaterial)
        {
            StringID    group;
            StringID    name;
            SerializeStringID(p_stream, group);
            SerializeStringID(p_stream, name);
            return ComponentManager::Get().AddComponent<MaterialComponent>(p_entityID, MaterialManager::Get().GetMaterial(name, group));
        }
        return ComponentManager::Get().AddComponent<MaterialComponent>(p_entityID);
    }

    void MaterialComponent::Expose()
    {
        ssq::Class sqMaterial = ScriptsHandler::Get().GetVirtualMachine().addClass("MaterialComponent", ssq::Class::Ctor<MaterialComponent(EntityID, ComponentID)>());
        sqMaterial.addFunc("SetShader",         [](MaterialComponent* self, std::string shader) { self->GetMaterial()->SetShader(ShaderManager::Get().GetShader(operator""_sid(shader.c_str(), shader.size()))); });
        sqMaterial.addFunc("SetUniformVector3", [](MaterialComponent* self, std::string uniform, Maths::Vector3 p_value)    { self->GetMaterial()->SetUniform(uniform, p_value); });
        sqMaterial.addFunc("SetUniformFloat",   [](MaterialComponent* self, std::string uniform, float p_value)             { self->GetMaterial()->SetUniform(uniform, p_value); });
    }
}