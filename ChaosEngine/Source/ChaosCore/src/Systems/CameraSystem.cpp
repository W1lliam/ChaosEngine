#include "ChaosCore/Systems/CameraSystem.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Components/CameraComponent.h"
#include "ChaosCore/Inputs/Input.h"
#include "ChaosCore/Resources/Managers/ShaderManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
    CHAOS_DEFINE_SYSTEM_TYPEID(CameraSystem);

    CameraSystem::CameraSystem(EntityID p_mainCameraID) : System(System::NORMAL_SYSTEM_PRIORITY, E_SystemExecutionModeFlags::GAME)
    {
        Expose();
		if (!ShaderManager::Get().matrixBuffer)
			ShaderManager::Get().matrixBuffer = Graphics::Buffers::IUniformBuffer::Create(0, 2 * sizeof(Maths::Matrix4) + sizeof(Maths::Vector3));


        if (p_mainCameraID != 0)
        {
            m_mainCameraID = p_mainCameraID;
            CameraComponent* camComp = ComponentManager::Get().GetComponent<CameraComponent>(p_mainCameraID);
            if (camComp)
            {
                ComponentManager::Get().GetComponent<TransformComponent>(p_mainCameraID)->AsTransform().SetOrientation(Maths::Vector3(camComp->GetYaw(), camComp->GetPitch(), 0), 2, 1, 3);
                BuildView();
            }
        }
    }

	void CameraSystem::Start()
	{
		m_mainCameraID = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene()->begin<CameraComponent>()->second->GetOwnerID();
	}

	void CameraSystem::UpdateProjection()
	{
		ShaderManager::Get().matrixBuffer->UpdateData(0, sizeof(Maths::Matrix4), const_cast<float*>(ComponentManager::Get().GetComponent<CameraComponent>(m_mainCameraID)->BuildProjection().Transpose().elements));
        m_projectionInitialized = true;
	}

	void CameraSystem::BuildView()
	{
        CameraComponent*    camComp         = ComponentManager::Get().GetComponent<CameraComponent>(m_mainCameraID);
        TransformComponent* transformComp   = ComponentManager::Get().GetComponent<TransformComponent>(m_mainCameraID);
        Maths::Matrix4      camTransform    = SceneSystem::GetWorldTransform(m_mainCameraID);
        Maths::Quaternion   camRotation     = transformComp->AsTransform().GetRotationQuaternion();
        Maths::Vector4      camPosition(camTransform(0,3), camTransform(1, 3),camTransform(2, 3),camTransform(3, 3));

        camComp->GetRight()   = (camRotation.RotateAntiClockWise(Maths::Vector3(1, 0, 0))).Normalized();
        camComp->GetUp()      = (camRotation.RotateAntiClockWise(Maths::Vector3(0, 1, 0))).Normalized();
        camComp->GetFront()   = (camRotation.RotateAntiClockWise(Maths::Vector3(0, 0, 1))).Normalized() * -1.f;

        camComp->GetViewMatrix() = Maths::Matrix4::CreateLookAt(
        camPosition.x,
		camPosition.y,
		camPosition.z,
        camPosition.x + camComp->GetFront().x, camPosition.y + camComp->GetFront().y, camPosition.z - camComp->GetFront().z,
        camComp->GetUp().x, camComp->GetUp().y, camComp->GetUp().z);
        camComp->GetViewMatrix() = camComp->GetViewMatrix().Transpose();
		
		ShaderManager::Get().matrixBuffer->UpdateData(sizeof(Maths::Matrix4), sizeof(Maths::Matrix4), camComp->GetViewMatrix().elements);
		ShaderManager::Get().matrixBuffer->UpdateData(2 * sizeof(Maths::Matrix4), sizeof(Maths::Vector3), camPosition.elements);
	}

	void CameraSystem::Update(double p_dt)
    {
        if (m_mainCameraID != 0)
        {
            if (!m_projectionInitialized)
            {
                UpdateProjection();
            }
		    BuildView();
        }
    }

    void CameraSystem::SetMainCamera(EntityID p_camera)
    {
        m_mainCameraID = p_camera;
        m_projectionInitialized = false;
    }

    void CameraSystem::SetMainCamera(GameObject* p_camera)
    {
        m_mainCameraID = p_camera->GetEntityID();
        m_projectionInitialized = false;
    }

    GameObject* CameraSystem::GetMainCameraGameObject() const
    {
        return static_cast<GameObject*>(EntityManager::Get().GetEntity(m_mainCameraID).get());
    }

    void CameraSystem::Expose()
    {
        ssq::Table cameraSystemTable = ScriptsHandler::Get().GetVirtualMachine().addTable("CameraSystem");
        cameraSystemTable.addFunc("GetMainCamera", [&]() -> AEntity* { return GetMainCameraGameObject(); });
        cameraSystemTable.addFunc("SetMainCamera", [&](GameObject* p_camera) { return SetMainCamera(p_camera); });
    }
    bool CameraSystem::Save(WriteStream& p_stream)
    {
        SerializeStringID(p_stream, m_mainCameraID);
        SerializeVector3(p_stream, m_target);
        System::Save(p_stream);
        return false;
    }
    System& CameraSystem::Load(ReadStream& p_stream)
    {
        StringID cameraID;
        SerializeStringID(p_stream, cameraID);
        
        CameraSystem* system = SystemManager::Get().GetSystem<CameraSystem>().get();
        if (system)
        {
            system->m_mainCameraID = cameraID;
        }
        else
        {
            system = SystemManager::Get().AddSystem<CameraSystem>(cameraID);
        }

        SerializeVector3(p_stream, system->m_target);
        static_cast<System*>(system)->Load(p_stream);
        return *system;
    }
}