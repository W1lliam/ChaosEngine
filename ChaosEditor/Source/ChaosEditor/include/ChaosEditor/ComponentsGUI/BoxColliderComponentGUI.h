#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"
#include "ChaosEditor/Inspector/DragSpinBox.h"
class QGroupBox;
class QCheckBox;

namespace ChaosEditor
{
	class Vector3Widget;
	class BoxColliderComponentGUI : public AComponentGUI
	{
		Q_OBJECT
	public:
		BoxColliderComponentGUI(QWidget* p_parent = nullptr);
		~BoxColliderComponentGUI() = default;

		virtual void SetupFrame() override;
		virtual void BindComponent(Chaos::IComponent* p_component) override;

	public slots:
		void HalfExtendUIChanged(double p_x, double p_y, double p_z);
		void FrictionUIChanged(double p_staticFriction, double p_dynamicFriction, double p_restitution);
		void TriggerUIChanged(bool p_triggerEnabled);
		void CenterUIChanged(double p_x, double p_y, double p_z);

        void CheckInput(const QString& p_dialogBox);
        void UpdateColliderMaskFilter(const QString& p_MaskName, bool p_add);
        void UpdateColliderMaskSelector(const QString& p_MaskName, bool p_add);
        void CheckOption();
        QGroupBox* CreateCollisionMaskFiltering();
        QComboBox* CreateCollisionMaskSelector();

	protected:
		Chaos::BoxColliderComponent*     m_component{ nullptr };
		Vector3Widget*				     m_friction{ nullptr };
		Vector3Widget*					 m_center{ nullptr };
		Vector3Widget*					 m_halfExtends{ nullptr };
		QComboBox*						 m_colliderCollisionMask{ nullptr };
		QGroupBox*						 m_collisionMaskFilter{ nullptr };
		QCheckBox*						 m_trigger{ nullptr };
	};
}