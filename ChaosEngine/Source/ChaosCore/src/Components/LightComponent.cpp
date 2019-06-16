#include "ChaosCore/Components/LightComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(LightComponent);

	LightComponent::LightComponent(COMPONENT_INIT_PARAMS) : COMPONENT_INIT(LightComponent)
	{

	}

    bool LightComponent::Save(WriteStream & p_stream)
    {
        SerializeUnsignedInt(p_stream, (uint32_t&)m_type, (uint32_t)E_LightType::DIRECTIONAL, (uint32_t)E_LightType::TYPE_COUNT);
        SerializeFloat(p_stream, m_intensity);
        SerializeVector4(p_stream, m_color);
        SerializeFloat(p_stream, m_angle);
        SerializeFloat(p_stream, m_falloff);
        SerializeFloat(p_stream, m_range);

        return true;
    }

    IComponent& LightComponent::Load(ReadStream & p_stream, const EntityID & p_entityID)
    {
        LightComponent& comp = ComponentManager::Get().AddComponent<LightComponent>(p_entityID);
        SerializeUnsignedInt(p_stream, (uint32_t&)comp.m_type, (uint32_t)E_LightType::DIRECTIONAL, (uint32_t)E_LightType::TYPE_COUNT);
        SerializeFloat(p_stream, comp.m_intensity);
        SerializeVector4(p_stream, comp.m_color);
        SerializeFloat(p_stream, comp.m_angle);
        SerializeFloat(p_stream, comp.m_falloff);
        SerializeFloat(p_stream, comp.m_range);
        return comp;
    }
    void LightComponent::Expose()
    {
        ssq::Enum sqLightType = ScriptsHandler::Get().GetVirtualMachine().addEnum("LightType");
        sqLightType.addSlot("DIRECTIONAL",  (int)E_LightType::DIRECTIONAL);
        sqLightType.addSlot("SPOT",         (int)E_LightType::SPOT);

        ssq::Class sqLight = ScriptsHandler::Get().GetVirtualMachine().addClass("LightComponent", ssq::Class::Ctor<LightComponent(EntityID, ComponentID)>());
        sqLight.addFunc("SetType",      [](LightComponent* self, int type) { self->SetType((E_LightType)type); });
        sqLight.addFunc("SetRange",     &LightComponent::SetRange);
        sqLight.addFunc("SetAngle",     &LightComponent::SetAngle);
        sqLight.addFunc("SetColor",     [](LightComponent* self, float r, float g, float b, float a) { self->SetColor({ r,g,b,a }); });
        sqLight.addFunc("SetIntensity", &LightComponent::SetIntensity);
        sqLight.addFunc("SetDirty",     &LightComponent::SetDirty);
    }
}