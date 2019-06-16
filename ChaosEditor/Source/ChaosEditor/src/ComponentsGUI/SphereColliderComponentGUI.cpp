#include "ChaosEditor/ComponentsGUI/SphereColliderComponentGUI.h"
#include "ChaosEditor/Inspector/Vector3Widget.h"
#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <qfloat16.h>
#include <QComboBox>
#include <qpushbutton.h>
#include <qinputdialog.h>
#include <qdir.h>

ChaosEditor::SphereColliderComponentGUI::SphereColliderComponentGUI(QWidget* p_parent) : AComponentGUI("SphereCollider", p_parent),
m_center(new Vector3Widget), m_friction(new Vector3Widget), m_trigger(new QCheckBox), m_radiusTracker(new DragSpinBox("", this))
{
	QObject::connect(m_center, &Vector3Widget::ValueChanged,
			this, &SphereColliderComponentGUI::CenterUIChanged);
	QObject::connect(m_friction, &Vector3Widget::ValueChanged,
			this, &SphereColliderComponentGUI::FrictionUIChanged);
	QObject::connect(m_trigger, &QCheckBox::stateChanged,
			this, &SphereColliderComponentGUI::TriggerUIChanged);
}

void ChaosEditor::SphereColliderComponentGUI::SetupFrame()
{
	QGridLayout* contentLayout = new QGridLayout;

	contentLayout->setSpacing(0);

	contentLayout->addWidget(new QLabel("Trigger"), 0, 0, Qt::AlignLeft);
	contentLayout->addWidget(m_trigger, 0, 1, Qt::AlignRight);

	contentLayout->addWidget(new QLabel("Friction"), 1, 0, Qt::AlignLeft);
	contentLayout->addWidget(m_friction, 1, 1, Qt::AlignRight);

	contentLayout->addWidget(new QLabel("Center"), 2, 0, Qt::AlignLeft);
	contentLayout->addWidget(m_center, 2, 1, Qt::AlignRight);

	contentLayout->addWidget(new QLabel("Radius"), 3, 0, Qt::AlignLeft);
	contentLayout->addWidget(m_radiusTracker, 3, 1, Qt::AlignRight);

	contentLayout->addWidget(new QLabel("Collider Mask"), 4, 0, 1, 3, Qt::AlignLeft);
	m_colliderCollisionMask = CreateCollisionMaskSelector();
	contentLayout->addWidget(m_colliderCollisionMask, 4, 1, 1, 3, Qt::AlignRight);

	contentLayout->addWidget(new QLabel("Collision Filtering"), 7, 0, 1, 1 , Qt::AlignCenter);
	m_collisionFilter = CreateCollisionMaskFiltering();
	contentLayout->addWidget(m_collisionFilter, 8, 0, 2, 40, Qt::AlignCenter);
	contentLayout->setColumnStretch(0, 1);
	contentLayout->setColumnStretch(1, 2);
	setLayout(contentLayout);
}

void ChaosEditor::SphereColliderComponentGUI::BindComponent(Chaos::IComponent * p_component)
{
	Chaos::SphereColliderComponent* colliderComponent = dynamic_cast<Chaos::SphereColliderComponent*>(p_component);
	CHAOS_ASSERT(colliderComponent, "Wrong component type!");

	m_center->SetLinkedVariable(&colliderComponent->GetPosition());
	m_friction->SetLinkedVariable(&colliderComponent->GetFrictionVector());
	m_radiusTracker->SetLinkedVariable(&colliderComponent->GetRadius());	
	m_component = colliderComponent;
}

void ChaosEditor::SphereColliderComponentGUI::FrictionUIChanged(double p_staticFriction, double p_dynamicFriction, double p_restitution)
{
	if (p_staticFriction >= 1)
		p_staticFriction = 1.f;

	else if (p_staticFriction <= 0)
		p_staticFriction = 0;
		p_staticFriction = 0;

	if (p_dynamicFriction >= 1)
		p_dynamicFriction = 1.f;

	else if (p_dynamicFriction <= 0)
		p_dynamicFriction = 0;

	if (p_restitution >= 1)
		p_restitution = 1.f;

	else if (p_restitution <= 0)
		p_restitution = 0;

	m_component->ChangeFriction(p_staticFriction, p_dynamicFriction, p_restitution);
}

void ChaosEditor::SphereColliderComponentGUI::TriggerUIChanged(bool p_triggerEnabled)
{
	m_component->GetPhysicsShape()->SetTrigger(p_triggerEnabled);
	m_trigger->setChecked(p_triggerEnabled);
}

void ChaosEditor::SphereColliderComponentGUI::CenterUIChanged(double p_x, double p_y, double p_z)
{
	m_component->ChangeOffset(p_x, p_y, p_z);
}

void ChaosEditor::SphereColliderComponentGUI::CollisionMaskUIChanged(QString p_string)
{
}

void ChaosEditor::SphereColliderComponentGUI::CollisionMaskFilteringUIChanged()
{
}

void ChaosEditor::SphereColliderComponentGUI::CheckInput(const QString & p_dialogBox)
{
	if (!p_dialogBox.isEmpty())
	{
		std::cout << "INPUT" << std::endl;
	}
}

void ChaosEditor::SphereColliderComponentGUI::UpdateColliderMaskFilter(const QString & p_MaskName, bool p_add)
{
	QVBoxLayout* vBox = static_cast<QVBoxLayout*>(m_collisionFilter->layout());
	QCheckBox* checkBox = new QCheckBox(p_MaskName);
	vBox->addWidget(checkBox);
	vBox->addStretch(3);
}

void ChaosEditor::SphereColliderComponentGUI::UpdateColliderMaskSelector(const QString & p_MaskName, bool p_add)
{
	m_colliderCollisionMask->addItem(p_MaskName);
}

void ChaosEditor::SphereColliderComponentGUI::CheckOption()
{

	bool ok;
	QInputDialog* dialogBox = new QInputDialog();
	dialogBox->setWindowTitle("type in new collision Mask Name");
	dialogBox->setLabelText("New Mask : ");
	QString text = dialogBox->getText(0, tr("New Collision Mask"),
		tr("Mask Name:"), QLineEdit::Normal,
		"", &ok);
	if (ok && !text.isEmpty())
	{
		UpdateColliderMaskFilter(text, true);
		UpdateColliderMaskSelector(text, true);
	}
}

QGroupBox * ChaosEditor::SphereColliderComponentGUI::CreateCollisionMaskFiltering()
{
	QGroupBox* groupBox = new QGroupBox(tr(""));
	groupBox->setFlat(true);
	QVBoxLayout* vBox = new QVBoxLayout;
	for (int i = 0; i < m_component->GetCollisionSetting().GetCollisionSetting().size(); ++i)
	{
		auto iterator = m_component->GetCollisionSetting().GetCollisionSetting().begin();
		iterator += i;
		std::string string = std::string(iterator->first);
		QCheckBox* checkBox = new QCheckBox(tr(string.c_str()));
		vBox->addWidget(checkBox);
	}
	QPushButton *pushButton = new QPushButton(tr("&Add Mask"));
	QObject::connect(pushButton, &QPushButton::pressed,
		this, &SphereColliderComponentGUI::CheckOption);

	vBox->addWidget(pushButton, 1, Qt::AlignCenter | Qt::AlignBottom);
	vBox->addStretch(1);
	groupBox->setLayout(vBox);
	return groupBox;
}

QComboBox * ChaosEditor::SphereColliderComponentGUI::CreateCollisionMaskSelector()
{
	QComboBox* comboBox = new QComboBox();
	for (int i = 0; i < m_component->GetCollisionSetting().GetCollisionSetting().size(); ++i)
	{
		auto iterator = m_component->GetCollisionSetting().GetCollisionSetting().begin();
		iterator += i;
		std::string string = std::string(iterator->first);
		comboBox->addItem(string.c_str());
	}
	return comboBox;
}

void ChaosEditor::SphereColliderComponentGUI::RadiusUIChanged(double p_radius)
{
	m_component->SetRadius(p_radius);
}