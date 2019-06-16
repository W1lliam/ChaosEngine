#pragma once
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/Components/Parameters/CameraComponentParams.h"
#include <ChaosMaths.h>

#ifdef CHAOS_EDITOR
namespace ChaosEditor { class CameraComponentGUI; }
#endif

namespace Chaos
{
    class CameraComponent : public IComponent
    {
        CHAOS_COMPONENT
#ifdef CHAOS_EDITOR
        friend class ::ChaosEditor::CameraComponentGUI;
#endif
    public:
        CameraComponent(COMPONENT_INIT_PARAMS);

        const Maths::Matrix4& BuildProjection();
        
		inline const Maths::Matrix4&    GetProjectionMatrix()   const { return	m_cameraParams.CameraViewParams.ProjectionMatrix;       }
		inline Maths::Matrix4&			GetViewMatrix()				  { return	m_cameraParams.CameraUpdateParams.ViewMatrix;           }
		inline Maths::Vector3&			GetTarget()					  { return	m_cameraParams.CameraUpdateParams.TargetPosition;       }
		inline E_CameraType&			GetType()					  { return	m_cameraParams.CameraModeSettings.CameraType;		    }
		inline E_CameraMovementMode&	GetMode()					  { return	m_cameraParams.CameraModeSettings.CameraMovementMode;	}
		inline E_CameraPerspectiveMode& GetPerspectiveMode()		  { return  m_cameraParams.CameraModeSettings.CameraPerspectiveMode;}
		inline E_CameraMovementOption&  GetMovementOption()			  { return  m_cameraParams.CameraModeSettings.CameraMovementOption; }

		inline float			GetFOV()		const		 { return	m_cameraParams.CameraViewParams.FOV;			   }
		inline float			GetAspect()		const		 { return	m_cameraParams.CameraViewParams.Aspect;			   }
		inline float			GetNear()		const		 { return	m_cameraParams.CameraViewParams.Near;			   }
		inline float			GetFar()		const		 { return	m_cameraParams.CameraViewParams.Far;			   }
		inline float			GetOrthographicScale() const { return   m_cameraParams.CameraViewParams.OrthographicScale; }

		inline bool&			IsFirstMouse()			{ return	m_cameraParams.CameraMovementParams.FirstMouse;		}
		inline bool&			IsTurbo()				{ return	m_cameraParams.CameraMovementParams.TurboMode;		}
		inline float&			GetSpeed()				{ return	m_cameraParams.CameraMovementParams.Speed;			}
		inline float&			GetSensitivity()		{ return	m_cameraParams.CameraMovementParams.Sensitivity;	}
		inline float&			GetAngleScale()			{ return	m_cameraParams.CameraMovementParams.AngleScale;		}
		inline float&			GetDistance()			{ return	m_cameraParams.CameraMovementParams.CameraDistance; }
		inline float&			GetAcceleration()		{ return	m_cameraParams.CameraMovementParams.Acceleration;	}

    	inline float&			GetYaw()				{ return	m_cameraParams.CameraUpdateParams.CurrentEulerAngles.x;	}
		inline float&			GetPitch()				{ return	m_cameraParams.CameraUpdateParams.CurrentEulerAngles.y;	}
		inline float&			GetRoll()				{ return    m_cameraParams.CameraUpdateParams.CurrentEulerAngles.z; }

		inline Maths::Vector3&	GetFront()				{ return	m_cameraParams.CameraUpdateParams.CameraFront;	      }
		inline Maths::Vector3&	GetRight()				{ return	m_cameraParams.CameraUpdateParams.CameraRight;	      }
		inline Maths::Vector3&	GetUp()					{ return	m_cameraParams.CameraUpdateParams.CameraUp;		      }
		inline Maths::Vector3&	GetNextPosition()		{ return	m_cameraParams.CameraUpdateParams.NextPosition;		  }
		inline Maths::Vector3&	GetNextEulerAngles()	{ return	m_cameraParams.CameraUpdateParams.NextRotationAngles; }

		void SetTarget(const Maths::Vector3& p_target);
		void SetType(E_CameraType p_type);

        virtual bool        Save(WriteStream& p_stream) override;
        static  IComponent& Load(ReadStream& p_stream, const EntityID& p_entityID);
        static  void Expose();
    private:
		CameraParams m_cameraParams;
    };
}