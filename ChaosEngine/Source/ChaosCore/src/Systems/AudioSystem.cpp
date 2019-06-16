#include "ChaosCore/Systems/AudioSystem.h"
#include "ChaosCore/Components/AudioSourceComponent.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Systems/CameraSystem.h"

namespace Chaos
{
	CHAOS_DEFINE_SYSTEM_TYPEID(AudioSystem);

	AudioSystem::AudioSystem() : System(System::NORMAL_SYSTEM_PRIORITY, E_SystemExecutionModeFlags::GAME)
	{
		m_studioSystem = NULL;
		AudioResourcesManager::Get().ErrorCheck(FMOD::Studio::System::create(&m_studioSystem));
		AudioResourcesManager::Get().ErrorCheck(m_studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));
	}
	void AudioSystem::Update(double p_dt)
	{
		std::vector<Resources::AudioChannel*> pStoppedChannels;


		Maths::Matrix4 camTransform = SceneSystem::GetWorldTransform(SystemManager::Get().GetSystem<CameraSystem>()->GetMainCameraID());

		Maths::Quaternion   camRotation(camTransform);
		Maths::Vector3      camPosition(camTransform(0, 3), camTransform(1, 3), camTransform(2, 3));

		Maths::Vector3 up(camRotation.RotateAntiClockWise(Maths::Vector3(0, 1, 0)).Normalized());
		Maths::Vector3 front(camRotation.RotateAntiClockWise(Maths::Vector3(0, 0, 1)).Normalized() * -1.0f);


		FMOD_VECTOR fvecPos;
		fvecPos.x = camPosition.x;
		fvecPos.y = camPosition.y;
		fvecPos.z = camPosition.z;

		FMOD_VECTOR fvecFor;
		fvecFor.x = front.x;
		fvecFor.y = front.y;
		fvecFor.z = front.z;

		FMOD_VECTOR fvecUp;
		fvecUp.x = up.x;
		fvecUp.y = up.y;
		fvecUp.z = up.z;

		AudioResourcesManager::Get().m_system->set3DListenerAttributes(0, &fvecPos, NULL, &fvecFor, &fvecUp);

		ScenePtr scene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
        for (auto audioIt = scene->begin<AudioSourceComponent>(); audioIt != scene->end<AudioSourceComponent>(); audioIt++)
		{
			AudioSourceComponent* audioSource = static_cast<AudioSourceComponent*>(audioIt->second);

			Resources::AudioChannel* mChannel = audioSource->GetChannel();
			if (mChannel)
			{
				AudioResourcesManager::Get().m_system->set3DSettings(1.0f, 1.0f, 1.0f);
				audioSource->UpdatePosition();

				if (!audioSource->IsPlaying())
				{
					pStoppedChannels.push_back(mChannel);
				}
			}
		}

		AudioResourcesManager::Get().ErrorCheck(m_studioSystem->update());
		AudioResourcesManager::Get().m_system->update();
	}
	bool AudioSystem::Save(WriteStream & p_stream)
	{
		System::Save(p_stream);
		return false;
	}

	System& AudioSystem::Load(ReadStream & p_stream)
	{
		auto system = SystemManager::Get().AddSystem<AudioSystem>();
		static_cast<System*>(system)->Load(p_stream);
		return *system;
	}
}
