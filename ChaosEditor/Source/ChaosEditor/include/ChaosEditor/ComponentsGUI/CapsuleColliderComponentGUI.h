#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"
#include "ChaosEditor/Inspector/DragSpinBox.h"

class QCheckBox;
namespace ChaosEditor
{
	class Vector3Widget;
	class CapsuleColliderComponentGUI : public AComponentGUI
	{
		Q_OBJECT
	public:
		CapsuleColliderComponentGUI(QWidget* p_parent = nullptr);
		~CapsuleColliderComponentGUI() = default;

	protected:
		Chaos::SphereColliderComponent*  m_component{ nullptr };
		Vector3Widget*				     m_friction{ nullptr };
		Vector3Widget*					 m_center{ nullptr };
		DragSpinBox*					 m_radiusTracker{ nullptr };
		DragSpinBox*					 m_halfHeightTracker{ nullptr };
		QCheckBox*						 m_trigger{ nullptr };
	};
}