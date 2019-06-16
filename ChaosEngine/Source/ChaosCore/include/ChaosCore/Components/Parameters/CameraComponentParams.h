#pragma once
#include "ChaosMaths/SIMDMaths/Vector/Vector3.h"
#include "ChaosMaths/SIMDMaths/Matrix/Matrix4x4.h"
#include "ChaosMaths/SIMDMaths/Quaternion/Quaternion.h"

namespace Chaos
{
	enum class E_CameraType : uint8_t
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	enum class E_CameraMovementMode : uint8_t
	{
		STATIONARY,
		FLY,
		PAN,
		ORBIT
	};
	enum class E_CameraPerspectiveMode : uint8_t
	{
		FIRST_PERSON,
		THIRD_PERSON
	};
	enum class E_CameraMovementOption : uint8_t
	{
		SMOOTH,
		NORMAL
	};
	struct CameraModes
	{
		E_CameraType CameraType = E_CameraType::PERSPECTIVE;
		E_CameraMovementMode CameraMovementMode = E_CameraMovementMode::FLY;
		E_CameraPerspectiveMode CameraPerspectiveMode = E_CameraPerspectiveMode::FIRST_PERSON;
		E_CameraMovementOption CameraMovementOption = E_CameraMovementOption::NORMAL;
	};
	
	struct CameraZoomParams
	{
		float Scale = 0.5f;
		float MinimumDistance = 1.0f;
		float MaximumDistance = 40.f;
	};
		
	struct CameraViewParams
	{
		float FOV = 60.0f;
		float Aspect = 16.0f / 9.0f;
		float Near = 0.1f;
		float Far = 1000.0f;
		float OrthographicScale = 4.0f;
		Maths::Matrix4 ProjectionMatrix;
	};

	struct CameraMovementParams
	{
		bool FirstMouse = true;
		bool TurboMode = false;
		float Speed = 0.3f;
		float Acceleration = 2.f;
		float Sensitivity = 0.05f;
		float AngleScale = 0.2f;
		float CameraDistance = 30.f;
		Maths::Vector3 AxisAngleConstraints = Maths::Vector3(89.5f, 0.f, 0.f);
		CameraZoomParams CameraZoomParams;
	};

	struct CameraUpdateParams
	{
		Maths::Vector3 CurrentEulerAngles = Maths::Vector3(0.f, 0.f, 0.f);
		Maths::Vector3 CameraFront = Maths::Vector3(0.0f, 0.0f, -1.0f);
		Maths::Vector3 CameraUp = Maths::Vector3(0.0f, 1.0f, 0.0f);
		Maths::Vector3 CameraRight = Maths::Vector3(1.0f, 0.f, 0.f);
		Maths::Vector3 TargetPosition;
		Maths::Vector3 NextPosition;
		Maths::Vector3 NextRotationAngles;

		Maths::Matrix4 ViewMatrix;
		Maths::Matrix4 RotTransMatrix;
	};

	struct CameraParams
	{
		CameraModes             CameraModeSettings;
		CameraViewParams        CameraViewParams;
		CameraMovementParams    CameraMovementParams;
		CameraUpdateParams      CameraUpdateParams;
	};
}
