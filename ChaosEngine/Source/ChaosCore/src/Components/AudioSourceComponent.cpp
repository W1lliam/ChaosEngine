#pragma once
#include "ChaosCore/Components/AudioSourceComponent.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
	CHAOS_DEFINE_COMPONENT_TYPEID(AudioSourceComponent);
	void AudioSourceComponent::Play()
	{
		if (m_audioClip)
		{
			int nChannelId = AudioResourcesManager::Get().m_nextChannel++;

			m_channel = nullptr;
			AudioResourcesManager::Get().ErrorCheck(AudioResourcesManager::Get().m_system->playSound(m_audioClip->GetSound(), nullptr, false, &m_channel));

			if (m_channel)
			{
				
				const Resources::AudioProperties& properties =  m_audioClip->GetProperty();
				

				SetVolume(m_volume);
				m_channel->setPaused(false);

				AudioResourcesManager::Get().mChannels[nChannelId] = m_channel;
			}
		}
	}

	void AudioSourceComponent::UpdatePosition()
	{
		if (m_audioClip->GetProperty().is3d)
		{
			AudioResourcesManager::Get().ErrorCheck(m_audioClip->GetSound()->set3DMinMaxDistance(minDistance, maxDistance));

			CHAOS_ASSERT(ComponentManager::Get().GetComponent<TransformComponent>(m_owner_ID) != nullptr, "Error: no transform component found in Game Object");
			Maths::Vector3 pos = ComponentManager::Get().GetComponent<TransformComponent>(m_owner_ID)->AsTransform().GetPosition();
			SetPosition(pos);
		}
	}

	void AudioSourceComponent::Pause()
	{
		if (m_channel)
		{
			m_channel->setPaused(true);
		}
	}
	void AudioSourceComponent::Stop()
	{
		if (m_channel)
		{
			m_channel->stop();
		}
	}
	void AudioSourceComponent::SetLoopingMode(bool p_loop)
	{
		if (m_audioClip)
		{
			m_audioClip->SetLooping(p_loop);
		}
	}
	void AudioSourceComponent::SetSpaceMode(bool p_3d)
	{
		if (m_audioClip)
		{
			m_audioClip->Set3d(p_3d);
		}
	}
	void AudioSourceComponent::SetVolume(float p_volume)
	{
		m_volume = p_volume;

		if (m_channel)
		{
			AudioResourcesManager::Get().ErrorCheck(m_channel->setVolume(m_volume));
		}
	}
	void AudioSourceComponent::SetPosition(const Maths::Vector3& p_pos)
	{
		if (m_channel)
		{
			FMOD_VECTOR fpos = ChaosVectorToFMOD(p_pos);
			m_channel->set3DAttributes(&fpos, NULL);
		}
	}
	float AudioSourceComponent::GetVolume()
	{
		return VolumeToDb(m_volume);
	}
	Resources::AudioChannel* AudioSourceComponent::GetChannel()
	{
		return m_channel;
	}
	bool AudioSourceComponent::IsPlaying()
	{
		if (m_channel)
		{
			bool play = false;
			m_channel->isPlaying(&play);
			return play;
		}
		return false;
	}
	float AudioSourceComponent::DbToVolume(float db)
	{
		return std::powf(10.0f, 0.05f * db);
	}
	float AudioSourceComponent::VolumeToDb(float db)
	{
		return 20.0f * log10f(db);
	}
	FMOD_VECTOR AudioSourceComponent::ChaosVectorToFMOD(Maths::Vector3 p_vector)
	{
		FMOD_VECTOR fpos;
		fpos.x = p_vector.x;
		fpos.y = p_vector.y;
		fpos.z = p_vector.z;
		return fpos;
	}

	bool AudioSourceComponent::Save(WriteStream& p_stream)
	{
		SerializeFloat(p_stream, minDistance);
		SerializeFloat(p_stream, maxDistance);
		SerializeFloat(p_stream, m_volume);
		SerializeString(p_stream, const_cast<std::string&>(m_audioClip->GetClipName()));
		SerializeStringID(p_stream, const_cast<StringID&>(m_audioClip->GetGroup()));

		return false;
	}

	IComponent&	AudioSourceComponent::Load(ReadStream& p_stream, const EntityID& p_entityID)
	{
		float p_minDist, p_maxDist, p_volume;
		std::string p_clipName;
		StringID p_group;

		SerializeFloat(p_stream, p_minDist);
		SerializeFloat(p_stream, p_maxDist);
		SerializeFloat(p_stream, p_volume);
		SerializeString(p_stream, p_clipName);
		SerializeStringID(p_stream, p_group);

		AudioSourceComponent& audioComp = ComponentManager::Get().AddComponent<AudioSourceComponent>(p_entityID, AudioResourcesManager::Get().GetAudio(p_clipName, p_group));

		return audioComp;
	}

	void AudioSourceComponent::Expose()
	{
        ssq::Class sqAudioSource = ScriptsHandler::Get().GetVirtualMachine().addClass("AudioSourceComponent", ssq::Class::Ctor<AudioSourceComponent(EntityID, ComponentID)>());
        sqAudioSource.addFunc("Play", &AudioSourceComponent::Play);
        sqAudioSource.addFunc("Pause", &AudioSourceComponent::Pause);
        sqAudioSource.addFunc("Stop", &AudioSourceComponent::Stop);

        sqAudioSource.addFunc("SetLoopingMode", &AudioSourceComponent::SetLoopingMode);
        sqAudioSource.addFunc("SetSpaceMode", &AudioSourceComponent::SetSpaceMode);
        sqAudioSource.addFunc("SetVolume", &AudioSourceComponent::SetVolume);
        sqAudioSource.addFunc("SetPosition", &AudioSourceComponent::SetPosition);


        sqAudioSource.addFunc("GetVolume", &AudioSourceComponent::GetVolume);
        sqAudioSource.addFunc("IsPlaying", &AudioSourceComponent::IsPlaying);

        sqAudioSource.addFunc("SetAudioClip", [](AudioSourceComponent* p_self, std::string p_clipName, std::string p_clipGroup)
        {
            Resources::AudioClipPtr audioclip = std::reinterpret_pointer_cast<Resources::AudioClip>(AudioResourcesManager::Get().CreateOrRetrieve(p_clipName, operator""_sid(p_clipGroup.c_str(), p_clipGroup.size())).first);
            p_self->SetAudioClip(audioclip);
        });
	}
}
