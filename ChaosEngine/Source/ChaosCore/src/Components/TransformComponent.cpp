#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(TransformComponent);

    bool TransformComponent::Save(WriteStream & p_stream)
    {
        SerializeVector3(p_stream, m_transform.m_position);
        SerializeQuaternion(p_stream, m_transform.m_rotation);
        SerializeVector3(p_stream, m_transform.m_scale);
        return true;
    }

    IComponent& TransformComponent::Load(ReadStream & p_stream, const EntityID& p_entityID)
    {
        TransformComponent& comp = ComponentManager::Get().AddComponent<TransformComponent>(p_entityID);
        SerializeVector3(p_stream,      comp.m_transform.m_position);
        SerializeQuaternion(p_stream,   comp.m_transform.m_rotation);
        SerializeVector3(p_stream,      comp.m_transform.m_scale);
        comp.m_transform.SetDirty(true);
        return comp;
    }

    void TransformComponent::Expose()
    {
        ssq::Class sqTransform = ScriptsHandler::Get().GetVirtualMachine().addClass("TransformComponent", ssq::Class::Ctor<TransformComponent(EntityID, ComponentID)>());
        sqTransform.addFunc("GetPosition",  &TransformComponent::GetPositionRef);
        sqTransform.addFunc("GetRotation",  &TransformComponent::GetRotationRef);
        sqTransform.addFunc("GeScale",      &TransformComponent::GetScaleRef);

        sqTransform.addFunc("SetPosition", [](TransformComponent* self, float x, float y, float z)
        {
            self->AsTransform().SetPosition(x, y, z);
        });
        sqTransform.addFunc("Translate", [](TransformComponent* self, float x, float y, float z)
        {
            self->AsTransform().Translate(x, y, z);
        });

        sqTransform.addFunc("SetOrientation", [](TransformComponent* self, float x, float y, float z) 
        { 
            self->AsTransform().SetOrientation(x, y, z);
        });
        sqTransform.addFunc("Rotate", [](TransformComponent* self, float x, float y, float z) 
        { 
            self->AsTransform().Rotate(x, y, z);
        });

        sqTransform.addFunc("SetScale", [](TransformComponent* self, float x, float y, float z) 
        { 
            self->AsTransform().SetScale(x, y, z);
        });

        sqTransform.addFunc("SetPositionVec", [](TransformComponent* self, Maths::Vector3& p_position)
        {
            self->AsTransform().SetPosition(p_position);
        });
        sqTransform.addFunc("TranslateVec", [](TransformComponent* self, Maths::Vector3& p_translate)
        {
            self->AsTransform().Translate(p_translate);
        });

        sqTransform.addFunc("SetOrientationVec", [](TransformComponent* self, Maths::Vector3& p_orientation)
        {
            self->AsTransform().SetOrientation(p_orientation);
        });
        sqTransform.addFunc("RotateVec", [](TransformComponent* self, Maths::Vector3& p_rotation)
        {
            self->AsTransform().Rotate(p_rotation);
        });

        sqTransform.addFunc("SetScaleVec", [](TransformComponent* self, Maths::Vector3& p_scale)
        {
            self->AsTransform().SetScale(p_scale);
        });
        sqTransform.addFunc("Forward", [](TransformComponent* self)
        {
            return self->AsTransform().Forward();
        });
        sqTransform.addFunc("Right", [](TransformComponent* self)
        {
            return self->AsTransform().Right();
        });
        sqTransform.addFunc("Up", [](TransformComponent* self)
        {
            return self->AsTransform().Up();
        });
        sqTransform.addFunc("LookAt", [](TransformComponent* self, Maths::Vector3& p_eye, Maths::Vector3& p_target )
        {
            Maths::Vector3 forward = self->AsTransform().Forward();
            auto transformMat =  Maths::Matrix4::CreateLookAt(  p_eye.x,    p_eye.y,    p_eye.z,
                p_target.x, p_target.y, p_target.z,
                                                                0.0f,       1.0f,       0.0f);
            self->AsTransform().SetTransform(transformMat);
        });
    }
}