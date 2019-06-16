#pragma once
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"
#include "fmod.hpp"

namespace Chaos
{
	class AudioResourcesManager;
}


namespace Chaos::Resources
{
	using	AudioSound		= FMOD::Sound;
	using	AudioChannel	= FMOD::Channel;

	struct AudioProperties
	{
		AudioProperties() = default;
		AudioProperties(const AudioProperties& p_props) : is3d(p_props.is3d), looping(p_props.looping), isStream(p_props.isStream)
		{}
		AudioProperties(bool p_is3d, bool p_isLooping, bool p_isStream) : is3d(p_is3d), looping(p_isLooping), isStream(p_isStream)
		{}

		bool is3d		= false;
		bool looping	= false;
		bool isStream	= false;
	};

	class AudioClip : public AResource
	{
		friend class AudioResourcesManager;
	public:
		AudioClip(AResourceManager* p_creator, const std::string& p_id, const StringID& p_group);
		~AudioClip();

		inline AudioSound*				GetSound()		{ return m_clip;		}
		inline const AudioProperties&	GetProperty()	{ return m_property;	}
		inline const std::string&		GetClipName()	{ return m_clipName;	}

		void SetLooping(bool p_looping);
		void Set3d(bool p_is3d);
		void SetStreaming(bool p_isStreaming);
		void SetProperty(AudioProperties p_property);

	protected:
		virtual void PrepareImpl()      override;
		virtual void LoadImpl()         override;

		virtual void UnprepareImpl()    override;

		virtual void UnloadImpl()       override;

		virtual void PostLoadImpl()     override;

	protected:

		static      Memory::PoolAllocator    s_allocator;

		std::string		m_clipName;
		AudioSound*		m_clip				{ nullptr };
		AudioProperties m_property;

	};
	using	AudioClipPtr	= std::shared_ptr<AudioClip>;
}