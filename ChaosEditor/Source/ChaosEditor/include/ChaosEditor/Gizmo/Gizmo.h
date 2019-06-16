#pragma

#include <ChaosCore.h>

enum class E_GizmoType
{
	NONE,
	MOVE,
	ROTATE,
	SCALE
};

struct Ray
{
	Chaos::Maths::Vector3 origin;
	Chaos::Maths::Vector3 direction;

	Ray() {}
	Ray(const Chaos::Maths::Vector3& p_origin, const Chaos::Maths::Vector3& p_direction) 
	{
		origin		= p_origin;
		direction	= p_direction;
	}
};

class Gizmo
{
public:
	Gizmo();
	~Gizmo();

	void Draw();
	
	inline void SetVisible(bool p_visible)								{ m_visible			= p_visible;		}
	inline void SetCameraPosition(Chaos::Transform& p_camTransform)		{ m_camTransform	= &p_camTransform;	}
	inline bool HasCamera()												{ return m_camTransform != nullptr;		}
	inline void SetGizmoType(E_GizmoType p_type)						{ m_gizmoType		= p_type;			}
	inline void SetTransform(Chaos::Transform& p_transform)				{ m_transform		= p_transform;		}
	inline void SetSelected(Chaos::EntityPtr p_entity)					{ m_selected		= p_entity;			}
	inline void SetSelectedColor(Chaos::Maths::Vector4 p_color)			{ m_selectedColor	= p_color;			}
	inline void ToggleGizmoLocal()										{ m_local			= !m_local;			}
	inline Chaos::Transform& GetTransform()								{ return m_transform;					}


private:

	void DrawMoveGizmo(float distanceFromCamera);
	void DrawScaleGizmo(float distanceFromCamera);
	void DrawCircle(float distanceFromCamera);

	bool  Intersects(const Ray& _ray, Chaos::Maths::Vector3 p_pointA, Chaos::Maths::Vector3 p_pointB);

public:
	Ray ray;

private:

	Chaos::Transform		m_transform;
	E_GizmoType				m_gizmoType		{ E_GizmoType::NONE };
	bool					m_visible		{ true };
	bool					m_local			{ false };
	bool					m_intersects	{ false };

	Chaos::EntityPtr		m_selected		{ nullptr };

	Chaos::Transform*		m_camTransform	{ nullptr };
	Chaos::Maths::Vector4	m_selectedColor { 1.0f, 1.0f, 0.0f, 1.0f };
};