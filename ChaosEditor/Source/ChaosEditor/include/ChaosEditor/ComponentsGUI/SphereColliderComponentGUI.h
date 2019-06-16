#pragma once
#include <ChaosEngine.h>
#include <qgroupbox.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"
#include "ChaosEditor/Inspector/DragSpinBox.h"
class QCheckBox;
class QInputDialog;
namespace ChaosEditor
{	
	class Vector3Widget;
	class SphereColliderComponentGUI : public AComponentGUI
	{
		Q_OBJECT
	public:
		SphereColliderComponentGUI(QWidget* p_parent = nullptr);
		~SphereColliderComponentGUI() = default;

		virtual void SetupFrame() override;
		virtual void BindComponent(Chaos::IComponent* p_component) override;

	public slots:
		void RadiusUIChanged(double p_radius);
		void FrictionUIChanged(double p_staticFriction, double p_dynamicFriction, double p_restitution);
		void TriggerUIChanged(bool p_triggerEnabled);
		void CenterUIChanged(double p_x, double p_y, double p_z);
		void CollisionMaskUIChanged(QString p_string);
		void CollisionMaskFilteringUIChanged();
		void CheckInput(const QString& p_dialogBox);
		void UpdateColliderMaskFilter(const QString& p_MaskName, bool p_add);
		void UpdateColliderMaskSelector(const QString& p_MaskName, bool p_add);
		void CheckOption();
		QGroupBox* CreateCollisionMaskFiltering();
		QComboBox* CreateCollisionMaskSelector();
	protected:
		Chaos::SphereColliderComponent*  m_component{ nullptr };
		Vector3Widget*				     m_friction{ nullptr };
		Vector3Widget*					 m_center{ nullptr };
		DragSpinBox*					 m_radiusTracker{ nullptr };
		QCheckBox*						 m_trigger{ nullptr };
		QComboBox*						 m_colliderCollisionMask{ nullptr };
		QGroupBox*						 m_collisionFilter{ nullptr };
	};
}