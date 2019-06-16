#pragma once
#include "ChaosCore/ECS/System/System.h"
#include <map>
#include <vector>

#define MAX_LIGHTS 256
namespace Chaos
{
	class LightComponent;


	struct Attenuation
	{
		float linear;
		float quadratic;
	};

	class LightSystem : public System
	{
        CHAOS_SYSTEM(LightSystem);
	public:
		LightSystem();
		~LightSystem() = default;

		virtual void Update(double p_dt) override;

		static Attenuation GetAttenuation(float p_range);

        static System& Load(ReadStream & p_stream);

	private:
		static const std::map<float, Attenuation> ATTENUATION;

	};
}