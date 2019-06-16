#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "AssetFileConstants.h"
#include "ChaosCore/Resources/AudioClip.h"

namespace Chaos
{
	class AudioResourcesManager : public Singleton<AudioResourcesManager>, public AResourceManager
	{
	public:

		AudioResourcesManager(const StringID& p_name = "AudioResourcesManager"_sid);
		~AudioResourcesManager();

		int		ErrorCheck(FMOD_RESULT p_result);

		Resources::AudioClipPtr CreateAudio(const std::string& p_name, const StringID& p_group, const char* p_path, bool p_is3d, bool p_isLooping, bool p_isStream);
		Resources::AudioClipPtr GetAudio(const std::string& p_name, const StringID& p_group = "Audio"_sid);

	protected:
		virtual AResource*		CreateImpl(const std::string& p_name, const StringID& p_group	= "Audio"_sid) override;
		virtual void			RemoveImpl(AResource* p_resource) override;
		virtual void			RemoveAllImpl();

	protected:
		static constexpr AssetType RESOURCE_TYPE{ AssetType::E_ASSETTYPE_AUDIO };

	public:
		static constexpr    uint8_t                        MAX_AUDIO_COUNT{ 36 };

		int		m_nextChannel{0};

		typedef std::map<std::string, Resources::AudioSound*>		SoundMap;
		typedef std::map<int, Resources::AudioChannel*>				ChannelMap;

		SoundMap	mClips;
		ChannelMap	mChannels;

		FMOD::System*			m_system;
	};
}
