#pragma once
#include "ChaosCore/GameObjects/CameraGameObject.h"
#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Events/KeyEvents.h"

namespace Chaos
{
	class CameraComponent;

    class CameraSystem : public System, public Events::EventListener
    {
        CHAOS_SYSTEM(CameraSystem);
    public:
        CameraSystem(EntityID p_mainCameraID = StringID());
        ~CameraSystem() = default;

		virtual void Start() override;

		void UpdateProjection();
		void BuildView();

        virtual void Update(double p_dt) override;

        void                SetMainCamera(EntityID p_camera);
        void                SetMainCamera(GameObject* p_camera);
        GameObject*         GetMainCameraGameObject()   const;
		inline EntityID     GetMainCameraID()   const { return m_mainCameraID; }

        void Expose();

        virtual bool        Save(WriteStream& p_stream) override;
        static  System&     Load(ReadStream& p_stream);

    private:
		Maths::Vector3		m_target;

        StringID            m_mainCameraID          {};
        bool                m_projectionInitialized {false};
    };
}