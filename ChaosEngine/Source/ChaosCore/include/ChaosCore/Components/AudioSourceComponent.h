#pragma once
#include "ChaosCore/Resources/Managers/AudioResourcesManager.h"
#include "ChaosCore/ECS/Component/Component.h"


#ifdef CHAOS_EDITOR
namespace ChaosEditor { class AudioSourceComponentGUI; }
#endif


namespace Chaos
{
	class AudioSourceComponent : public IComponent
	{
		CHAOS_COMPONENT

#ifdef  CHAOS_EDITOR
			friend class ::ChaosEditor::AudioSourceComponentGUI;
#endif //  CHAOS_EDITOR
#pragma region Methods
	public:
		AudioSourceComponent(COMPONENT_INIT_PARAMS, Resources::AudioClipPtr p_audio = {}) : COMPONENT_INIT(AudioSourceComponent), m_audioClip(p_audio)
		{

		}

		~AudioSourceComponent() = default;

		inline void SetAudioClip(Chaos::Resources::AudioClipPtr p_audioClip) { m_audioClip = p_audioClip; }
		inline Resources::AudioClipPtr GetAudioClip() { return m_audioClip; }

		void Play();
		void Pause();
		void Stop();

		void SetLoopingMode(bool p_loop);
		void SetSpaceMode(bool p_3d);
		void SetVolume(float p_volume);
		void SetPosition(const Maths::Vector3& p_pos);

		float GetVolume();
		Resources::AudioChannel* GetChannel();
		bool IsPlaying();

		void UpdatePosition();

		virtual bool		Save(WriteStream& p_stream) override;
		static IComponent&	Load(ReadStream& p_stream, const EntityID& p_entityID);
		static void			Expose();

	protected:

	private:

		float DbToVolume(float db);
		float VolumeToDb(float db);

		FMOD_VECTOR ChaosVectorToFMOD(Maths::Vector3 p_vector);

#pragma endregion

#pragma region Members
	public:
		float minDistance = 0.5f;
		float maxDistance = 100.0f;


	protected:

	private:
		Chaos::Resources::AudioClipPtr				m_audioClip	{ nullptr };
		Resources::AudioChannel*					m_channel	{ nullptr };

		float			m_volume;

#pragma endregion		
	};
}