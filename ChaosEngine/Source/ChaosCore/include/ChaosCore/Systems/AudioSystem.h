#pragma once
#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/Scene/Scene.h"
#include "fmod_studio.hpp"

namespace Chaos
{
	class AudioSystem : public System
	{
		CHAOS_SYSTEM(AudioSystem);
		
#pragma region Methods
	public:
		AudioSystem();
		~AudioSystem() = default;

		virtual void Update(double p_dt) override;

		virtual bool	 Save(WriteStream& p_stream) override;
		static  System&  Load(ReadStream& p_stream);

	protected:

	private:

#pragma endregion

#pragma region Members
	public:

	protected:
		FMOD::Studio::System* m_studioSystem{nullptr};

	private:

#pragma endregion

	};
}