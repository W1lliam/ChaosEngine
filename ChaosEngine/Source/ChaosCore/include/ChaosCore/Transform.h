#pragma once
#include <ChaosMaths.h>
#include "TransformBase.h"

namespace Chaos
{
	class Transform : public ATransform
	{
    friend class TransformComponent;
	private:
		Maths::Quaternion   m_rotation;
		Maths::Vector3      m_position;
		
		Maths::Vector3      m_up		= Maths::Vector3(0.0f,1.0f,0.0f);
		Maths::Vector3      m_right		= Maths::Vector3(1.0f, 0.0f, 0.0f);
		Maths::Vector3      m_forward	= Maths::Vector3(0.0f, 0.0f, -1.0f);

		bool                m_dirty     = true;

	public:
		Transform();
		~Transform() override = default;
		Transform(const Maths::Vector3& p_position, const Maths::Quaternion& p_quaternion);

		Transform operator*(const Transform& p_other) const;
		Transform& operator*=(const Transform& p_other);

		void Translate(const float x, const float y, const float z) override;
		void Translate(const Maths::Vector3& p_dir) override;

		void SetPosition(const float x, const float y, const float z) override;
		void SetPosition(const Maths::Vector3& p_pos) override;

		void SetXPosition(const float p_pos) override;
		void SetYPosition(const float p_pos) override;
		void SetZPosition(const float p_pos) override;

		void Rotate(float x, float y, float z, int p_xOrder = 2, int p_yOrder = 1, int p_zOrder = 3) override;
		void Rotate(const Maths::Vector3& p_rotation, int p_xOrder = 2, int p_yOrder = 1, int p_zOrder = 3) override;
		void Rotate(const Maths::Quaternion& p_rotation) override;

		void SetOrientation(const float x, const float y, const float z, int p_xOrder = 2, int p_yOrder = 1, int p_zOrder = 3) override;
		void SetOrientation(const Maths::Vector3& p_rotation, int p_xOrder = 2, int p_yOrder = 1, int p_zOrder = 3) override;
		void SetOrientation(const float x, const float y, const float z, const float w) override;
		void SetOrientation(const Maths::Quaternion& p_rotation) override;

		void LookAt(const Maths::Vector3& p_targetPosition) override;
		void LookAt(const Maths::Vector3& p_targetPosition, const Maths::Vector3& p_up) override;
		void LookAtFromQuaternion(const Maths::Vector3& p_targetPosition) override;

		const Maths::Vector3& GetPosition() const override;
		Maths::Vector3 GetPosition() override;
		const Maths::Vector3 GetOrientation() const override;
		const Maths::Quaternion& GetRotationQuaternion() const;
		Maths::Vector3 Up() override;
		Maths::Vector3 Right() override;
		Maths::Vector3 Forward() override;
		void SetTransform(const Maths::Matrix4& p_transform) override;

		Maths::Matrix4 GetTranslation() override;
		Maths::Matrix4 GetRotation() override;
		Maths::Matrix4 GetTransform() override;
		Maths::Vector3 TransformVector(const Maths::Vector3& p_toTransform) override;
		Chaos::Transform TransformFromParent(const Transform& p_parent) const;
		Chaos::Transform InverseTransformFromParent(const Transform& p_parent) const;
		Chaos::Transform GetInverse() const;

		inline bool Chaos::Transform::IsDirty()				{ return m_dirty;	}
		inline void Chaos::Transform::SetDirty(bool p_b)	{ m_dirty = p_b;	}
    };
}
