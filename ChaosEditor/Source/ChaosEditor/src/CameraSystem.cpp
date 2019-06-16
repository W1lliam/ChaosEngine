#include "ChaosEditor/CameraSystem.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/Components/CameraComponent.h"

#include "ChaosCore/Resources/Managers/ShaderManager.h"
#include "ChaosEditor/InputHandler.h"
#include "KeyCodes.h"
#include "QGuiApplication"
#include "QCursor"

namespace ChaosEditor
{

	using namespace Chaos;
    CHAOS_DEFINE_SYSTEM_TYPEID(CameraSystem);

    CameraSystem::CameraSystem() : 
        System(System::NORMAL_SYSTEM_PRIORITY, E_SystemExecutionModeFlags::EDITOR)
    {
        Expose();
        if (!ShaderManager::Get().matrixBuffer)
            ShaderManager::Get().matrixBuffer = Graphics::Buffers::IUniformBuffer::Create(0, 2 * sizeof(Maths::Matrix4) + sizeof(Maths::Vector3));

        m_mainCameraGameObject = EntityManager::Get().CreateEntity<CameraGameObject>("EditorCamera"_sid).get();
        m_mainCameraComponent = m_mainCameraGameObject->GetComponent<CameraComponent>();

        TransformComponent* transformComp = m_mainCameraGameObject->GetComponent<TransformComponent>();
        transformComp->AsTransform().SetOrientation(Maths::Vector3(m_mainCameraComponent->GetYaw(), m_mainCameraComponent->GetPitch(), 0), 2, 1, 3);
        transformComp->AsTransform().SetPosition(0.0f, 3.0f, -5.0f);
        RegisterEventCallback<Chaos::Events::MouseButtonPressedEvent>(std::bind(&CameraSystem::OnMouseButtonPressed, this, std::placeholders::_1));
        RegisterEventCallback<Chaos::Events::MouseButtonReleasedEvent>(std::bind(&CameraSystem::OnMouseButtonReleased, this, std::placeholders::_1));

        UpdateProjection();
        BuildView();
    }

    void CameraSystem::UpdateProjection()
    {
        ShaderManager::Get().matrixBuffer->UpdateData(0, sizeof(Maths::Matrix4), const_cast<float*>(m_mainCameraComponent->BuildProjection().Transpose().elements));
    }

    void CameraSystem::BuildView()
    {
        m_mainCameraComponent->GetRight()   = m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().Right();
        m_mainCameraComponent->GetUp()      = m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().Up();
        m_mainCameraComponent->GetFront()   = m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().Forward() * -1.f;
        if (m_mainCameraComponent->GetMode() == E_CameraMovementMode::ORBIT)
        {
            m_mainCameraComponent->GetViewMatrix() = m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetRotationQuaternion().ToMatrix4();
            m_mainCameraComponent->GetViewMatrix().m03 = -m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition().x;
            m_mainCameraComponent->GetViewMatrix().m13 = -m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition().y;
            m_mainCameraComponent->GetViewMatrix().m23 = -m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition().z;
        }
        else
        {
            m_mainCameraComponent->GetViewMatrix() = Maths::Matrix4::CreateLookAt(m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition().x,
                m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition().y,
                m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition().z,
                m_mainCameraComponent->GetTarget().x, m_mainCameraComponent->GetTarget().y, m_mainCameraComponent->GetTarget().z,
                m_mainCameraComponent->GetUp().x, m_mainCameraComponent->GetUp().y, m_mainCameraComponent->GetUp().z);
        }
        m_mainCameraComponent->GetViewMatrix() = m_mainCameraComponent->GetViewMatrix().Transpose();

        ShaderManager::Get().matrixBuffer->UpdateData(sizeof(Maths::Matrix4), sizeof(Maths::Matrix4), m_mainCameraComponent->GetViewMatrix().elements);
        ShaderManager::Get().matrixBuffer->UpdateData(2 * sizeof(Maths::Matrix4), sizeof(Maths::Vector3), m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition().elements);
    }

    void CameraSystem::KeyBoardMovement(double p_dt) const
    {
        float xStep, yStep, zStep;
        xStep = yStep = zStep = 0;
        const float steps = 2.0f;
        if (Input::Get().IsKeyPressed(E_ChaosKeyCode::CHAOS_KEY_D)) xStep += steps;
        if (Input::Get().IsKeyPressed(E_ChaosKeyCode::CHAOS_KEY_A)) xStep -= steps;
        if (Input::Get().IsKeyPressed(E_ChaosKeyCode::CHAOS_KEY_W)) zStep += steps;
        if (Input::Get().IsKeyPressed(E_ChaosKeyCode::CHAOS_KEY_S)) zStep -= steps;
        if (Input::Get().IsKeyPressed(E_ChaosKeyCode::CHAOS_KEY_E)) yStep += steps;
        if (Input::Get().IsKeyPressed(E_ChaosKeyCode::CHAOS_KEY_Q)) yStep -= steps;

        m_mainCameraComponent->GetNextPosition() = m_mainCameraComponent->GetRight() *  xStep + m_mainCameraComponent->GetFront() * zStep + m_mainCameraComponent->GetUp() * yStep;
        m_mainCameraComponent->GetNextPosition() *= m_mainCameraComponent->IsTurbo() ? (m_mainCameraComponent->GetSpeed() * 50.0f) *  (float)p_dt : m_mainCameraComponent->GetSpeed() *  (float)p_dt;

        m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().Translate(m_mainCameraComponent->GetNextPosition());
        m_mainCameraComponent->GetNextPosition() = Maths::Vector3();
    }

    void CameraSystem::MouseMovement(double p_dt) const
    {
        float xoffset = Input::Get().GetMouseX() - m_mouseMoveStartPos.first;
        float yoffset = Input::Get().GetMouseY() - m_mouseMoveStartPos.second;
        if (xoffset != 0 || yoffset != 0)
        {

            xoffset *= m_mainCameraComponent->GetSensitivity();
            yoffset *= m_mainCameraComponent->GetSensitivity();

            Maths::Vector3 CurrentAngles = m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetRotationQuaternion().ToEulerAngles();
            m_mainCameraComponent->GetNextEulerAngles().y -= xoffset;
            m_mainCameraComponent->GetNextEulerAngles().x -= yoffset;
            m_mainCameraComponent->GetNextEulerAngles().z = 0;

            if (CurrentAngles.x + m_mainCameraComponent->GetNextEulerAngles().x > 87.0f)
                m_mainCameraComponent->GetNextEulerAngles().x = 0;
            if (CurrentAngles.x + m_mainCameraComponent->GetNextEulerAngles().x < -87.0f)
                m_mainCameraComponent->GetNextEulerAngles().x = 0;

            m_mainCameraComponent->GetNextEulerAngles().x += CurrentAngles.x;
            m_mainCameraComponent->GetNextEulerAngles().y += CurrentAngles.y;

            m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().SetOrientation(m_mainCameraComponent->GetNextEulerAngles());
            m_mainCameraComponent->GetNextEulerAngles() = Maths::Vector3(0, 0, 0);
            
            Input::Get().SetMousePosition(m_mouseMoveStartPos.first, m_mouseMoveStartPos.second);
        }
    }

    void CameraSystem::CameraInputUpdate(double p_dt)
    {
        if (Input::Get().IsKeyPressed(E_ChaosKeyCode::CHAOS_KEY_LEFT_SHIFT))
        {
            m_mainCameraComponent->IsTurbo() = true;
        }
        else if (Input::Get().IsKeyReleased(E_ChaosKeyCode::CHAOS_KEY_LEFT_SHIFT))
        {
            m_mainCameraComponent->IsTurbo() = false;
        }
        if (m_isMouse2Pressed && Input::Get().IsKeyReleased(E_ChaosKeyCode::CHAOS_KEY_LEFT_ALT))
        {
            m_mainCameraComponent->GetMode() = E_CameraMovementMode::FLY;
            MouseMovement(p_dt);
            KeyBoardMovement(p_dt);
        }
        if (m_isMouse1Pressed && Input::Get().IsKeyPressed(E_ChaosKeyCode::CHAOS_KEY_LEFT_ALT))
        {
            m_mainCameraComponent->GetMode() = E_CameraMovementMode::ORBIT;
            MouseMovement(p_dt);
        }

        if (Input::Get().IsKeyReleased(E_ChaosKeyCode::CHAOS_KEY_LEFT_ALT) && m_isMouse3Pressed)
        {
            m_mainCameraComponent->GetMode() = E_CameraMovementMode::PAN;
            PanMouseMovement(p_dt);
        }

        if (Input::Get().IsKeyReleased(E_ChaosKeyCode::CHAOS_KEY_LEFT_ALT) &&
            !m_isMouse1Pressed &&
            !m_isMouse2Pressed &&
            !m_isMouse3Pressed)
        {
            m_mainCameraComponent->GetMode() = E_CameraMovementMode::STATIONARY;
        }
    }

    void CameraSystem::SetUpTarget() const
    {
        switch (m_mainCameraComponent->GetMode())
        {
        case E_CameraMovementMode::STATIONARY:
        case E_CameraMovementMode::PAN:
            m_mainCameraComponent->SetTarget(m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition() + m_mainCameraComponent->GetFront() * 10.0);
        case E_CameraMovementMode::FLY:
            m_mainCameraComponent->SetTarget(m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition() + m_mainCameraComponent->GetFront() * 5.0);
            break;
        case E_CameraMovementMode::ORBIT:
            m_mainCameraComponent->SetTarget(m_target);
            break;
        }
    }

    void CameraSystem::SmoothKeyBoardMovement(double p_dt) const
    {
        Maths::Vector3 currentPosition = m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().GetPosition();
    }

    void CameraSystem::SmoothMouseMovement(double p_dt) const
    {
    }

    void CameraSystem::ZoomToTargetPosition(double p_dt, Maths::Vector3 TargetPosition) const
    {
    }

    bool CameraSystem::OnMouseButtonPressed(const Chaos::Events::MouseButtonPressedEvent& p_event)
    {
        if (p_event.GetMouseButton() == E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_1)
        {
            m_isMouse1Pressed   = true;
        }
        if (p_event.GetMouseButton() == E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_2)
        {
            m_isMouse2Pressed   = true;
        }
        if (p_event.GetMouseButton() == E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_3)
        {
            m_isMouse3Pressed   = true;
        }
        m_mouseMoveStartPos = Input::Get().GetMousePos();
        Input::Get().HideMouse(true);
        return false;
    }

    bool CameraSystem::OnMouseButtonReleased(const Chaos::Events::MouseButtonReleasedEvent& p_event)
    {
        if (p_event.GetMouseButton() == E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_1)
        {
            m_isMouse1Pressed = false;
        }
        if (p_event.GetMouseButton() == E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_2)
        {
            m_isMouse2Pressed = false;
        }
        if (p_event.GetMouseButton() == E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_3)
        {
            m_isMouse3Pressed = false;
        }
        Input::Get().HideMouse(false);
        return false;
    }

    void CameraSystem::Update(double p_dt)
    {
        CameraInputUpdate(p_dt);
        SetUpTarget();
        BuildView();
    }

    void CameraSystem::PanMouseMovement(double p_dt) const
    {
        float xStep = Input::Get().GetMouseX() - m_mouseMoveStartPos.first;
        float yStep = Input::Get().GetMouseY() - m_mouseMoveStartPos.second;
        if (xStep != 0 || yStep != 0)
        {

            xStep *= m_mainCameraComponent->GetSensitivity();
            yStep *= m_mainCameraComponent->GetSensitivity();
            m_mainCameraComponent->GetNextPosition() = m_mainCameraComponent->GetRight() *  xStep + m_mainCameraComponent->GetUp() * -yStep;
            m_mainCameraComponent->GetNextPosition() *= m_mainCameraComponent->IsTurbo() ? (m_mainCameraComponent->GetSpeed() * 10.0f) *  (float)p_dt : m_mainCameraComponent->GetSpeed() *  (float)p_dt * 5.0f;

            m_mainCameraGameObject->GetComponent<TransformComponent>()->AsTransform().Translate(m_mainCameraComponent->GetNextPosition());
            m_mainCameraComponent->GetNextPosition() = Maths::Vector3();

            Input::Get().SetMousePosition(m_mouseMoveStartPos.first, m_mouseMoveStartPos.second);
        }
    }

    void CameraSystem::SetMainCamera(GameObject* p_camera)
    {
        m_mainCameraGameObject = p_camera;
        m_mainCameraComponent = p_camera->GetComponent<CameraComponent>();
    }

    GameObject* CameraSystem::GetMainCameraGameObject() const
    {
        return m_mainCameraGameObject;
    }
}
