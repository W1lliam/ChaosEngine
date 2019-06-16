#pragma once
#include <ChaosMaths.h>
namespace Chaos
{
	class ATransform
	{
	protected:
        Maths::Vector3 m_scale  {1.0f, 1.0f, 1.0f};
	public:
		ATransform() = default;
		virtual ~ATransform() = default;

		virtual void Translate(const float x, const float y, const float z) = 0;
		virtual void Translate(const Maths::Vector3& p_dir) = 0;

		virtual void SetPosition(const float x, const float y, const float z) = 0;
		virtual void SetPosition(const Maths::Vector3& p_pos) = 0;

		virtual void SetXPosition(const float p_pos) = 0;
		virtual void SetYPosition(const float p_pos) = 0;
		virtual void SetZPosition(const float p_pos) = 0;

		virtual void Rotate(float p_x, float p_y, float p_z, int p_xOrder, int p_yOrder, int p_zOrder) = 0;
		virtual void Rotate(const Maths::Vector3& p_rotation, int p_xOrder, int p_yOrder, int p_zOrder) = 0;
		virtual void Rotate(const Maths::Quaternion& p_rotation) = 0;

		virtual void SetOrientation(const float x, const float y, const float z, int p_xOrder, int p_yOrder, int p_zOrder) = 0;
		virtual void SetOrientation(const Maths::Vector3& p_rotation, int p_xOrder, int p_yOrder, int p_zOrder) = 0;
        virtual void SetOrientation(const float x, const float y, const float z, const float w) = 0;
		virtual void SetOrientation(const Maths::Quaternion& p_rotation) = 0;
		
		virtual void LookAt(const Maths::Vector3& p_targetPosition) = 0;
		virtual void LookAt(const Maths::Vector3& p_targetPosition, const Maths::Vector3& p_up) = 0;
		virtual void LookAtFromQuaternion(const Maths::Vector3& p_targetPosition) = 0;
		void Scale(float p_scale);
		void Scale(float x, float y, float z);
		void Scale(const Maths::Vector3& p_scale);

		void SetScale(float x, float y, float z);
		void SetScale(const Maths::Vector3& p_scale);

		virtual const Maths::Vector3& GetPosition() const = 0;
		virtual Maths::Vector3 GetPosition() = 0;
		const Maths::Vector3& GetScale() const;
		virtual const Maths::Vector3 GetOrientation() const = 0;
		virtual Maths::Vector3 Up() = 0;
		virtual Maths::Vector3 Right() = 0;
		virtual Maths::Vector3 Forward() = 0;
		virtual Maths::Vector3 TransformVector(const Maths::Vector3& p_toTransform) = 0;

		virtual void SetTransform(const Maths::Matrix4& p_transform) = 0;

		virtual Maths::Matrix4  GetTranslation() = 0;
		virtual Maths::Matrix4  GetRotation() = 0;
		Maths::Matrix4 GetScaling() const;
		virtual Maths::Matrix4  GetTransform() = 0;
	};
}