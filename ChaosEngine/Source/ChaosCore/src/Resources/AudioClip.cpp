#include "ChaosCore/Resources/AudioClip.h"
#include "ChaosCore/Resources/Managers/AudioResourcesManager.h"

namespace Chaos::Resources
{
	Chaos::Memory::PoolAllocator AudioClip::s_allocator{ sizeof(Resources::AudioClip) * AudioResourcesManager::MAX_AUDIO_COUNT, sizeof(Resources::AudioClip), alignof(Resources::AudioClip) };

	AudioClip::AudioClip(AResourceManager * p_creator, const std::string & p_id, const StringID & p_group):
		AResource(p_creator, operator""_sid(p_id.c_str(), p_id.size()), p_group), m_clipName(p_id)
	{

	}

	void AudioClip::PostLoadImpl()
	{

	}

	AudioClip::~AudioClip()
	{
		if (m_clip) m_clip->release();
	}

	void AudioClip::SetLooping(bool p_isLooping)
	{
		m_property.looping = p_isLooping;

		SetProperty(m_property);
	}

	void AudioClip::Set3d(bool p_is3d)
	{
		m_property.is3d = p_is3d;

		SetProperty(m_property);
	}

	void AudioClip::SetStreaming(bool p_isStreaming)
	{
		m_property.isStream = p_isStreaming;

		SetProperty(m_property);
	}

	void AudioClip::SetProperty(AudioProperties p_property)
	{
		FMOD_MODE eMode = FMOD_DEFAULT;

		eMode |= m_property.is3d		? FMOD_3D			: FMOD_2D;
		eMode |= m_property.looping		? FMOD_LOOP_NORMAL	: FMOD_LOOP_OFF;
		eMode |= m_property.isStream	? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

		if(m_clip)
			m_clip->setMode(eMode);
	}

	void AudioClip::PrepareImpl()
	{
	}

	void AudioClip::LoadImpl()
	{
		FMOD_MODE eMode = FMOD_DEFAULT;

		eMode |= m_property.is3d		? FMOD_3D			: FMOD_2D;
		eMode |= m_property.looping		? FMOD_LOOP_NORMAL	: FMOD_LOOP_OFF;
		eMode |= m_property.isStream	? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

		AudioResourcesManager::Get().ErrorCheck(AudioResourcesManager::Get().m_system->createSound(ResourceGroupManager::Get().GetResourcePath(m_name, m_group).c_str(), eMode, nullptr, &m_clip));

		if (m_clip)
		{
			AudioResourcesManager::Get().mClips[m_clipName] = m_clip;
		}
	}
	void AudioClip::UnprepareImpl()
	{
	}
	void AudioClip::UnloadImpl()
	{
	}
}