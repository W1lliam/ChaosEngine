#include"ChaosEditor/Gizmo/Gizmo.h"

Gizmo::Gizmo()
{
	m_transform.SetPosition(Chaos::Maths::Vector3(0.0f, 0.0f, 0.0f));
	m_transform.SetScale(Chaos::Maths::Vector3(1.0f, 1.0f, 1.0f));
}

Gizmo::~Gizmo()
{

}

void Gizmo::Draw()
{
	using namespace Chaos;

	Graphics::IRenderer::Get().Clear(Graphics::DEPTH_BIT);

	Transform rayTrans;
	rayTrans.SetPosition(ray.origin);

	
	Graphics::IRenderer::Get().DrawPoint(ray.direction, 5.0f, Maths::Vector4(1.0, 1.0, 1.0, 1.0));

	float distanceFromCamera;
	if (m_camTransform)
	{
		distanceFromCamera = (m_transform.GetPosition() - m_camTransform->GetPosition()).Length() * 0.1f;
	}
	else
		distanceFromCamera = 1.0f;

	if (m_selected)
	{
		m_visible = true;
		if (m_local)
		{

			Maths::Vector3 position = Maths::Vector3(SceneSystem::GetWorldTransform(m_selected->GetEntityID())(0, 3), SceneSystem::GetWorldTransform(m_selected->GetEntityID())(1, 3), SceneSystem::GetWorldTransform(m_selected->GetEntityID())(2, 3));
			m_transform.SetPosition(position);
            auto transformComp = m_selected->GetComponent<TransformComponent>();
            if(transformComp)
			    m_transform.SetOrientation(transformComp->AsTransform().GetRotationQuaternion());
		}
		else
		{
			m_transform = Transform();
			Maths::Vector3 position = Maths::Vector3(SceneSystem::GetWorldTransform(m_selected->GetEntityID())(0,3), SceneSystem::GetWorldTransform(m_selected->GetEntityID())(1, 3), SceneSystem::GetWorldTransform(m_selected->GetEntityID())(2, 3));
			m_transform.SetPosition(position);
		}
	}
	else
	{
		m_transform = Transform();
		m_visible = false;
	}

	if (!m_visible)
		return;

	switch (m_gizmoType)
	{
	case E_GizmoType::NONE:
		break;
	case E_GizmoType::MOVE:
	{
		DrawMoveGizmo(distanceFromCamera);
		break;
	}

	case E_GizmoType::SCALE:
	{
		DrawScaleGizmo(distanceFromCamera);
		break;
	}

	case E_GizmoType::ROTATE:
	{
		DrawCircle(distanceFromCamera);
		break;
	}
	}
}

void Gizmo::DrawMoveGizmo(float distanceFromCamera)
{
	using namespace Chaos;

	m_intersects = Intersects(ray,	m_transform.GetPosition() + (m_transform.Right() * (distanceFromCamera * 0.2f)),
									m_transform.GetPosition() + (m_transform.Right() * (distanceFromCamera + (distanceFromCamera * 0.8f))));

	bool m_intersectsy = Intersects(ray,	m_transform.GetPosition() + (m_transform.Up() * (distanceFromCamera * 0.2f)),
									m_transform.GetPosition() + (m_transform.Up() * (distanceFromCamera + (distanceFromCamera * 0.8f))));

	bool m_intersectsz = Intersects(ray,	m_transform.GetPosition() + (m_transform.Forward() * (distanceFromCamera * 0.2f)),
									m_transform.GetPosition() + (m_transform.Forward() * (distanceFromCamera + (distanceFromCamera * 0.8f))));

	Maths::Vector3 pos		= m_transform.GetPosition();
	Maths::Vector3 right	= m_transform.Right();
	Maths::Vector3 up		= m_transform.Up();
	Maths::Vector3 forward	= m_transform.Forward();

	//Middle point
	Graphics::IRenderer::Get().DrawPoint(pos, 10.0f, Maths::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	//Right Arrow
	Graphics::IRenderer::Get().DrawLine(pos + (right* (distanceFromCamera * 0.2f)), 8.0f, pos + right * (distanceFromCamera * 1.8f), 8.0f, m_intersects ? m_selectedColor : Maths::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	Graphics::IRenderer::Get().DrawLine(pos + (right * (distanceFromCamera * 1.8f)), 15.0f, pos + (right * (distanceFromCamera * 2.0f)), 2.0f, m_intersects ? m_selectedColor : Maths::Vector4(0.8f, 0.0f, 0.0f, 1.0f));
	Graphics::IRenderer::Get().DrawPoint(pos + (right * (distanceFromCamera * 1.85f)), 12.0f, m_intersects ? m_selectedColor : Maths::Vector4(0.8f, 0.0f, 0.0f, 1.0f));
	//Up Arrow
	Graphics::IRenderer::Get().DrawLine(pos + (up * (distanceFromCamera * 0.2f)), 8.0f, pos + (up * (distanceFromCamera * 1.8f)), 8.0f, m_intersectsy ? m_selectedColor : Maths::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	Graphics::IRenderer::Get().DrawLine(pos + (up * (distanceFromCamera * 1.8f)), 15.0f, pos + (up * (distanceFromCamera * 2.0f)), 2.0f, m_intersectsy ? m_selectedColor : Maths::Vector4(0.0f, 0.8f, 0.0f, 1.0f));
	Graphics::IRenderer::Get().DrawPoint(pos + (up * (distanceFromCamera * 1.85f)), 12.0f, m_intersectsy ? m_selectedColor : Maths::Vector4(0.0f, 0.8f, 0.0f, 1.0f));
	//Forward Arrow
	Graphics::IRenderer::Get().DrawLine(pos + (forward * (distanceFromCamera * 0.2f)), 8.0f, pos + (forward * (distanceFromCamera * 1.8f)), 8.0f, m_intersectsz ? m_selectedColor : Maths::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	Graphics::IRenderer::Get().DrawLine(pos + (forward * (distanceFromCamera * 1.8f)), 15.0f, pos + (forward * (distanceFromCamera * 2.0f)), 2.0f, m_intersectsz ? m_selectedColor : Maths::Vector4(0.0f, 0.0f, 0.8f, 1.0f));
	Graphics::IRenderer::Get().DrawPoint(pos + (forward * (distanceFromCamera * 1.85f)), 12.0f, m_intersectsz ? m_selectedColor : Maths::Vector4(0.0f, 0.0f, 0.8f, 1.0f));
}

void Gizmo::DrawScaleGizmo(float distanceFromCamera)
{
	using namespace Chaos;

	Maths::Vector3 pos		= m_transform.GetPosition();
	Maths::Vector3 right	= m_transform.Right();
	Maths::Vector3 up		= m_transform.Up();
	Maths::Vector3 forward	= m_transform.Forward();

	Graphics::IRenderer::Get().DrawLine(pos, 8.0f, pos + (right * (distanceFromCamera * 1.8f)), 8.0f, Maths::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	Graphics::IRenderer::Get().DrawPoint(pos + (right * (distanceFromCamera * 1.8f)), 20.0f, Maths::Vector4(0.8f, 0.0f, 0.0f, 1.0f));
	Graphics::IRenderer::Get().DrawLine(pos, 8.0f, pos + (up * (distanceFromCamera * 1.8f)), 8.0f, Maths::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	Graphics::IRenderer::Get().DrawPoint(pos + (up * (distanceFromCamera * 1.8f)), 20.0f, Maths::Vector4(0.0f, 0.8f, 0.0f, 1.0f));
	Graphics::IRenderer::Get().DrawLine(pos, 8.0f, pos + (forward * (distanceFromCamera * 1.8f)), 8.0f, Maths::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	Graphics::IRenderer::Get().DrawPoint(pos + (forward * (distanceFromCamera * 1.8f)), 20.0f, Maths::Vector4(0.0f, 0.0f, 0.8f, 1.0f));

}

void Gizmo::DrawCircle(float distanceFromCamera)
{
	using namespace Chaos;

	Maths::Vector3 rot = m_transform.GetRotationQuaternion().ToEulerAngles();

	Maths::Vector3 pos = m_transform.GetPosition();

	Graphics::IRenderer::Get().DrawCircle(pos, rot + Maths::Vector3(90.0f, 0.0f, 0.0f), distanceFromCamera, 48, Maths::Vector4(1.0f, 0.0f, 0.0f, 1.0f), 5.0f);
	Graphics::IRenderer::Get().DrawCircle(pos, rot + Maths::Vector3(0.0f,90.0f,0.0f), distanceFromCamera, 48, Maths::Vector4(0.0f, 1.0f, 0.0f, 1.0f), 5.0f);
	Graphics::IRenderer::Get().DrawCircle(pos, rot + Maths::Vector3(0.0f,0.0f,90.0f), distanceFromCamera, 48, Maths::Vector4(0.0f, 0.0f, 1.0f, 1.0f), 5.0f);
}

bool Gizmo::Intersects(const Ray & p_ray, Chaos::Maths::Vector3 p_pointA, Chaos::Maths::Vector3 p_pointB)
{
	using namespace Chaos;

	Maths::Vector3 da = p_ray.direction - p_ray.origin;
	Maths::Vector3 db = p_pointB - p_pointA;
	Maths::Vector3 dc = p_pointA - p_ray.origin;

	if (std::abs(dc.Dot(da.Cross(db))) >= 0.7f)
		return false;

	double s = dc.Cross(db).Dot(da.Cross(db)) / std::pow(da.Cross(db).Length(), 2.0f);

	if (s >= 0.0 && s <= 1.0)
	{
		Maths::Vector3 intersection = p_ray.origin + (da * (float)s);

		
		if (std::pow((intersection - p_pointA).Length(), 2.0f) + std::pow((intersection - p_pointB).Length(), 2.0f) <= std::pow((p_pointB - p_pointA).Length(), 2.0f) + 1e-3f)
			return true;
	}

	return false;
}
