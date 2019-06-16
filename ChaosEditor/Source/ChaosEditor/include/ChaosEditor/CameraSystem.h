#pragma once
#include "ChaosEngine.h"

namespace Chaos
{
	class CameraComponent;
}
namespace ChaosEditor
{

    class CameraSystem : public Chaos::System, public Chaos::Events::EventListener
    {
        CHAOS_SYSTEM(CameraSystem);
    public:
        CameraSystem();
        ~CameraSystem() = default;

		void UpdateProjection();
		void BuildView();

		void PanMouseMovement(double p_dt) const;
		void KeyBoardMovement(double p_dt) const;
		void MouseMovement(double p_dt) const;
		void CameraInputUpdate(double p_dt);
		void SetUpTarget() const;

		void SetMainCamera(Chaos::GameObject* p_camera);

		void SmoothKeyBoardMovement(double p_dt) const;
		void SmoothMouseMovement(double p_dt) const;
		void ZoomToTargetPosition(double p_dt, Chaos::Maths::Vector3 TargetPosition) const;

        bool OnMouseButtonPressed(const Chaos::Events::MouseButtonPressedEvent& p_event);
        bool OnMouseButtonReleased(const Chaos::Events::MouseButtonReleasedEvent& p_event);

		virtual void Update(double p_dt) override;

		Chaos::GameObject* GetMainCameraGameObject() const;

	private:
		Chaos::Maths::Vector3		m_target;

        std::pair<float, float> m_mouseMoveStartPos {0.0f,0.0f};
        bool m_isMouse1Pressed {false};
        bool m_isMouse2Pressed {false};
        bool m_isMouse3Pressed {false};

		Chaos::CameraComponent* m_mainCameraComponent{ nullptr };
		Chaos::GameObject*      m_mainCameraGameObject{ nullptr };
	};
}