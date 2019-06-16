#include "ChaosCore/Components/CameraComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(CameraComponent);

    CameraComponent::CameraComponent(COMPONENT_INIT_PARAMS) : COMPONENT_INIT(CameraComponent)
    {

    }

    void CameraComponent::SetTarget(const Maths::Vector3& p_target)
    {
        m_cameraParams.CameraUpdateParams.TargetPosition = p_target;
    }

    void CameraComponent::SetType(E_CameraType p_type)
    {
        m_cameraParams.CameraModeSettings.CameraType = p_type;
    }

    bool CameraComponent::Save(WriteStream & p_stream)
    {
        SerializeFloat(p_stream, m_cameraParams.CameraViewParams.FOV);
        SerializeFloat(p_stream, m_cameraParams.CameraViewParams.Near);
        SerializeFloat(p_stream, m_cameraParams.CameraViewParams.Far);
        uint8_t type = static_cast<uint8_t>(m_cameraParams.CameraModeSettings.CameraType);
        SerializeUnsignedInt(p_stream, type, (uint8_t)E_CameraType::PERSPECTIVE, (uint8_t)E_CameraType::ORTHOGRAPHIC);
        return true;
    }

    IComponent& CameraComponent::Load(ReadStream & p_stream, const EntityID & p_entityID)
    {
        CameraComponent& comp = ComponentManager::Get().AddComponent<CameraComponent>(p_entityID);
        SerializeFloat(p_stream, comp.m_cameraParams.CameraViewParams.FOV);
        SerializeFloat(p_stream, comp.m_cameraParams.CameraViewParams.Near);
        SerializeFloat(p_stream, comp.m_cameraParams.CameraViewParams.Far);
        uint8_t type;
        SerializeUnsignedInt(p_stream, type, (uint8_t)E_CameraType::PERSPECTIVE, (uint8_t)E_CameraType::ORTHOGRAPHIC);
        comp.m_cameraParams.CameraModeSettings.CameraType = static_cast<E_CameraType>(type);
        return comp;
    }

    void CameraComponent::Expose()
    {
        ssq::Class sqCamera = ScriptsHandler::Get().GetVirtualMachine().addClass("CameraComponent", ssq::Class::Ctor<CameraComponent(EntityID, ComponentID)>());
        sqCamera.addFunc("SetTarget", &CameraComponent::SetTarget);
    }

    const Maths::Matrix4& CameraComponent::BuildProjection()
    {
        if (GetType() == E_CameraType::PERSPECTIVE)
        {
			m_cameraParams.CameraViewParams.ProjectionMatrix = Maths::Matrix4::CreatePerspective(GetFOV(), GetAspect(), GetNear(), GetFar());
        }
        else if(GetType() == E_CameraType::ORTHOGRAPHIC)
        {
			m_cameraParams.CameraViewParams.ProjectionMatrix = Maths::Matrix4::CreateOrthographic(GetOrthographicScale(), GetAspect(), GetNear(), GetFar());
        }
        return GetProjectionMatrix();
    }
}