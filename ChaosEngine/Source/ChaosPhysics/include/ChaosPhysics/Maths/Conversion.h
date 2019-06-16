#pragma once
#include "PxPhysicsAPI.h"
#include <ChaosMaths.h>
#include "ChaosCore/Transform.h"

namespace Chaos
{
	namespace Physics
	{
		namespace Converter
		{
			static physx::PxVec3 ConvertVector3ToPxVec3(Maths::Vector3 p_vector3)
			{
				return physx::PxVec3(p_vector3.x, p_vector3.y, p_vector3.z);
			}

			static physx::PxVec4 ConvertVector4ToPxVec4(Maths::Vector4 p_vector4)
			{
				return physx::PxVec4(p_vector4.x, p_vector4.y, p_vector4.z, p_vector4.z);
			}
			

			static physx::PxQuat ConvertQuaternionToPxQuat(Maths::Quaternion p_quaternion)
			{
				return physx::PxQuat(p_quaternion.GetX(), p_quaternion.GetY(), p_quaternion.GetZ(), p_quaternion.GetRealValue());
			}

			static physx::PxTransform ConvertTransformToPxTransform(Transform p_transform)
			{
				physx::PxVec3 position = ConvertVector3ToPxVec3(p_transform.GetPosition());
				physx::PxQuat rotation = ConvertQuaternionToPxQuat(p_transform.GetRotationQuaternion().GetNormalized());
				return physx::PxTransform(position, rotation);
			}

			static Maths::Vector3 ConvertPxVec3ToVector3(physx::PxVec3 p_pxVec3)
			{
				return Maths::Vector3(p_pxVec3.x, p_pxVec3.y, p_pxVec3.z);
			}
			static Maths::Vector4 ConvertPxVec4ToVector4(physx::PxVec4 p_pxVec4)
			{
				return Maths::Vector4(p_pxVec4.x, p_pxVec4.y, p_pxVec4.z, p_pxVec4.w);
			}
			static Maths::Quaternion ConvertPxQuatToQuaternion(physx::PxQuat p_pxQuat)
			{
				return Maths::Quaternion(p_pxQuat.x, p_pxQuat.y, p_pxQuat.z, p_pxQuat.w);
			}
			static Transform ConvertPxTransformToTransform(physx::PxTransform p_pxTransform)
			{
				Maths::Vector3 position = ConvertPxVec3ToVector3(p_pxTransform.p);
				Maths::Quaternion rotation = ConvertPxQuatToQuaternion(p_pxTransform.q);
				return Transform(position, rotation);
			}
		}
	}
}