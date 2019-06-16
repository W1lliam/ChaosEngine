#pragma once
#include "ChaosMaths/SIMDMaths/Vector/Vector3.h"


namespace Chaos
{
	enum E_FrictionCombinedMode
	{
		AVERAGE,
		MIN,
		MULTIPLY,
		MAX
	};

	struct FrictionValue
	{
		union
		{
			struct
			{
				float dynamicFriction;
				float staticFriction;
				float restitution;
			};
			Maths::Vector3 FrictionValues;
		};

		FrictionValue()
		{
			dynamicFriction = 0;
			staticFriction = 0;
			restitution = 0;
		}
	};

	class PhysicsFriction
	{
	public:
		PhysicsFriction(float p_dynamic, float p_static, float p_restitution);

		E_FrictionCombinedMode GetFrictionMode() const;
		FrictionValue GetFrictionValues() const;
		float GetDynamicFriction() const;
		float GetStaticFriction() const;
		float GetRestitution() const;
		Maths::Vector3& GetFrictions();
		void SetFictionMode(E_FrictionCombinedMode p_mode);
		void SetFrictions(float staticFriction, float dynamicFriction, float restitution);
		void SetFrictionValue(FrictionValue p_values);
		void SetDynamicFriction(float p_dynamicFriction);
		void SetStaticFriction(float p_staticFriction);
		void SetRestitution(float p_restitution);

	private:
		E_FrictionCombinedMode m_frictionMode;
		FrictionValue m_frictionValues;
	};
}
