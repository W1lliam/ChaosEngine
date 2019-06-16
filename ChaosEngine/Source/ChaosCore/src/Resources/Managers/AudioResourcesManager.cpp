#include "ChaosCore/Resources/Managers/AudioResourcesManager.h"

namespace Chaos
{
	AudioResourcesManager::AudioResourcesManager(const StringID & p_name) : AResourceManager(p_name, RESOURCE_TYPE, 400)
	{

		m_system = NULL;
		AudioResourcesManager::ErrorCheck(FMOD::System_Create(&m_system));

		int driverCount = 0;
		m_system->getNumDrivers(&driverCount);

		CHAOS_ASSERT(driverCount > 0, "No audio drivers found");
		m_system->init(MAX_AUDIO_COUNT, FMOD_INIT_NORMAL, NULL);
	}

	AudioResourcesManager::~AudioResourcesManager()
	{
		RemoveAll();
	}

	int AudioResourcesManager::ErrorCheck(FMOD_RESULT p_result)
	{
		if (p_result != FMOD_OK)
		{
			CHAOS_CORE_ERROR("FMOD ERROR: {0} ", p_result);
			return 1;
		}
		return 0;
	}

	Resources::AudioClipPtr AudioResourcesManager::CreateAudio(const std::string& p_name, const StringID & p_group, const char* p_path, bool p_is3d, bool p_isLooping, bool p_isStream)
	{
		ResourcePtr resource(CreateImpl(p_name, p_group), std::bind(&AudioResourcesManager::RemoveImpl, this, std::placeholders::_1));
		std::reinterpret_pointer_cast<Resources::AudioClip>(resource)->SetProperty({ p_is3d , p_isLooping, p_isStream });
		AddImpl(resource);
		ResourceGroupManager::Get()._NotifyResourceCreated(resource);
		return std::static_pointer_cast<Resources::AudioClip>(resource);
	}
	Resources::AudioClipPtr AudioResourcesManager::GetAudio(const std::string & p_name, const StringID & p_group)
	{
		return std::static_pointer_cast<Resources::AudioClip>(GetResource(operator""_sid(p_name.c_str(), p_name.size()), p_group));
	}
	AResource* AudioResourcesManager::CreateImpl(const std::string& p_name, const StringID& p_group)
	{
		return new Resources::AudioClip(this, p_name, p_group);
	}

	void AudioResourcesManager::RemoveImpl(AResource * p_resource)
	{
		static_cast<Resources::AudioClip*>(p_resource)->~AudioClip();
		Resources::AudioClip::s_allocator.Free(p_resource);
	}
	void AudioResourcesManager::RemoveAllImpl()
	{
		Resources::AudioClip::s_allocator.Clear();
	}
}
