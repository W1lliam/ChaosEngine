#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"
#include "ChaosEditor/Inspector/DragSpinBox.h"
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qcombobox.h>
namespace ChaosEditor
{
	class Vector3Widget;
	class QuaternionWidget;

	class RigidBodyComponentGUI : public AComponentGUI
	{
		Q_OBJECT
	public:

		RigidBodyComponentGUI(QWidget* p_parent = nullptr);
		~RigidBodyComponentGUI() = default;

		virtual void SetupFrame() override;
		virtual void BindComponent(Chaos::IComponent* p_component) override;

	public slots:

		void MassUIChanged(float p_mass);
		void AngularDragUIChanged(float p_angularDrag);
		void LinearDragUIChanged(float p_linearDrag);
		void GravityUIChanged(bool p_enableGravity);
		void KinematicUIChanged(bool p_isKinematic);
		void XRotationConstraintUIChanged(bool p_x);
		void YRotationConstraintUIChanged(bool p_y);
		void ZRotationConstraintUIChanged(bool p_z);

		void XTranslationConstraintUIChanged(bool p_x);
		void YTranslationConstraintUIChanged(bool p_y);
		void ZTranslationConstraintUIChanged(bool p_z);
		void CollisionDetectionModeUIChanged(const QString& p_string);

		QGroupBox* CreateRotationConstraints();
		QGroupBox* CreateTranslationConstraints();
		QComboBox* CreateCollisionDetectionMode();

	protected:
		Chaos::RigidBodyComponent*  m_rigidbody{ nullptr };
		DragSpinBox*				m_mass{ nullptr };
		DragSpinBox*				m_angularDragTracker{ nullptr };
		DragSpinBox*				m_linearDragTracker{ nullptr };
		QCheckBox*					m_enableGravity{ nullptr };
		QCheckBox*					m_isKinematic{ nullptr };
		QGroupBox*					m_rotationConstraint{ nullptr };
		QGroupBox*					m_translationConstraint{ nullptr };
		QComboBox*					m_CollisionDetectionMode{ nullptr };
	};
}