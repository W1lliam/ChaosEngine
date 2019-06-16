#include "ChaosEditor/ComponentsGUI/ComponentGUIFactory.h"
#include "ChaosEditor/ComponentsGUI/TransformComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/LightComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/ChildComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/CameraComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/MaterialComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/MeshComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/SphereColliderComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/BoxColliderComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/AudioSourceComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/RigidBodyComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/ScriptedComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/AnimationComponentGUI.h"
namespace ChaosEditor
{
    AComponentGUI* ChaosEditor::ComponentGUIFactory::CreateComponentGUI(Chaos::ComponentTypeID p_componentType, QWidget* p_parent)
    {
        if (p_componentType == Chaos::TransformComponent::COMPONENT_TYPE_ID)
        {
            return new TransformComponentGUI(p_parent);
        }
        else if (p_componentType == Chaos::LightComponent::COMPONENT_TYPE_ID)
        {
            return new LightComponentGUI(p_parent);
        }
        else if (p_componentType == Chaos::CameraComponent::COMPONENT_TYPE_ID)
        {
            return new CameraComponentGUI(p_parent);
        }
        else if (p_componentType == Chaos::MaterialComponent::COMPONENT_TYPE_ID)
        {
            return new MaterialComponentGUI(p_parent);
        }
        else if (p_componentType == Chaos::MeshComponent::COMPONENT_TYPE_ID)
        {
            return new MeshComponentGUI(p_parent);
        }
		else if (p_componentType == Chaos::SphereColliderComponent::COMPONENT_TYPE_ID)
		{
			return new SphereColliderComponentGUI(p_parent);
		}
		else if (p_componentType == Chaos::BoxColliderComponent::COMPONENT_TYPE_ID)
		{
			return new BoxColliderComponentGUI(p_parent);
		}
		else if (p_componentType == Chaos::RigidBodyComponent::COMPONENT_TYPE_ID)
		{
			return new RigidBodyComponentGUI(p_parent);
		}
		else if (p_componentType == Chaos::AudioSourceComponent::COMPONENT_TYPE_ID)
		{
			return new AudioSourceComponentGUI(p_parent);
		}
		else if (p_componentType == Chaos::AnimationComponent::COMPONENT_TYPE_ID)
		{
			return new AnimationComponentGUI(p_parent);
		}
        else if (p_componentType == Chaos::ChildComponent::COMPONENT_TYPE_ID)
        {
            return nullptr;
        }
		else
		{
			return new ScriptedComponentGUI(p_componentType.string, p_parent);
		}
        return nullptr;
    }

    AComponentGUI * ComponentGUIFactory::CreateScriptedComponentGUI(const std::string& p_scriptedComponentType, QWidget * p_parent)
    {
        return new ScriptedComponentGUI(p_scriptedComponentType, p_parent);
    }
}
