#include "ChaosEditor\ComponentsGUI\RigidBodyComponentGUI.h"
#include <QHBoxLayout>

ChaosEditor::RigidBodyComponentGUI::RigidBodyComponentGUI(QWidget* p_parent) : AComponentGUI("RigidBody", p_parent),
m_mass(new DragSpinBox("")), m_angularDragTracker(new DragSpinBox("")),
m_linearDragTracker(new DragSpinBox("")), m_enableGravity(new QCheckBox()),
m_isKinematic(new QCheckBox())
{
	QObject::connect(m_mass, &DragSpinBox::ValueChanged,
		this, &RigidBodyComponentGUI::MassUIChanged);
	QObject::connect(m_angularDragTracker, &DragSpinBox::ValueChanged,
		this, &RigidBodyComponentGUI::AngularDragUIChanged);
	QObject::connect(m_linearDragTracker, &DragSpinBox::ValueChanged,
		this, &RigidBodyComponentGUI::LinearDragUIChanged);
	QObject::connect(m_enableGravity, &QCheckBox::stateChanged,
		this, &RigidBodyComponentGUI::GravityUIChanged);
	QObject::connect(m_isKinematic, &QCheckBox::stateChanged,
		this, &RigidBodyComponentGUI::KinematicUIChanged);

}

void ChaosEditor::RigidBodyComponentGUI::SetupFrame()
{
	QGridLayout* contentLayout = new QGridLayout;

	contentLayout->setSpacing(0);

	contentLayout->addWidget(new QLabel("Mass"), 0, 0, Qt::AlignLeft);
	contentLayout->addWidget(m_mass, 0, 1, Qt::AlignRight);

	contentLayout->addWidget(new QLabel("AngularDrag"), 1, 0, Qt::AlignLeft);
	contentLayout->addWidget(m_angularDragTracker, 1, 1, Qt::AlignRight);

	contentLayout->addWidget(new QLabel("LinearDrag"), 2, 0, Qt::AlignLeft);
	contentLayout->addWidget(m_linearDragTracker , 2, 1, Qt::AlignRight);

	contentLayout->addWidget(new QLabel("UseGravity"), 3, 0, 1, 2, Qt::AlignLeft);
	contentLayout->addWidget(m_enableGravity, 3, 1, 1, 2, Qt::AlignRight);
	
	contentLayout->addWidget(new QLabel("Kinematic"), 5, 0, 1, 2, Qt::AlignLeft);
	contentLayout->addWidget(m_isKinematic, 5, 1, 1, 2, Qt::AlignRight);
	
	contentLayout->addWidget(new QLabel("Rotation Constraints"), 7, 0, Qt::AlignLeft);
	m_rotationConstraint = CreateRotationConstraints();
	contentLayout->addWidget(m_rotationConstraint, 7, 1, 1, 2, Qt::AlignRight);
	
	contentLayout->addWidget(new QLabel("Translation Constraints"), 8, 0, Qt::AlignLeft);
	m_translationConstraint = CreateTranslationConstraints();
	contentLayout->addWidget(m_translationConstraint, 8, 1, 1, 2, Qt::AlignRight);

	contentLayout->addWidget(new QLabel("Collision Detection Mode"), 9, 0, Qt::AlignLeft);
	m_CollisionDetectionMode = CreateCollisionDetectionMode();
	contentLayout->addWidget(m_CollisionDetectionMode, 9, 1, Qt::AlignRight);
	
	setLayout(contentLayout);
}

void ChaosEditor::RigidBodyComponentGUI::BindComponent(Chaos::IComponent * p_component)
{
	Chaos::RigidBodyComponent* rigidBody = dynamic_cast<Chaos::RigidBodyComponent*>(p_component);
	CHAOS_ASSERT(rigidBody, "Wrong component type!");

	m_mass->SetLinkedVariable(&rigidBody->GetMass());
	m_angularDragTracker->SetLinkedVariable(&rigidBody->GetAngularDrag());
	m_linearDragTracker->SetLinkedVariable(&rigidBody->GetLinearDrag());
	if (rigidBody->Actor().GetisKinematic())
		m_isKinematic->setChecked(true);
	
	if (rigidBody->IsGravityEnabled())
		m_enableGravity->setChecked(true);

	m_rigidbody = rigidBody;
}

void ChaosEditor::RigidBodyComponentGUI::MassUIChanged(float p_mass)
{
	if(m_rigidbody)
	m_rigidbody->SetMass(p_mass);
}
void ChaosEditor::RigidBodyComponentGUI::AngularDragUIChanged(float p_angularDrag)
{
	if (m_rigidbody)
	m_rigidbody->SetAngularDrag(p_angularDrag);
}

void ChaosEditor::RigidBodyComponentGUI::LinearDragUIChanged(float p_linearDrag)
{
	if (m_rigidbody)
	m_rigidbody->SetLinearDrag(p_linearDrag);
}

void ChaosEditor::RigidBodyComponentGUI::GravityUIChanged(bool p_enableGravity)
{
	if (m_rigidbody)
	m_rigidbody->SetGravity(p_enableGravity);
}

void ChaosEditor::RigidBodyComponentGUI::KinematicUIChanged(bool p_isKinematic)
{
	if (m_rigidbody)
	m_rigidbody->SetKinematic(p_isKinematic);
}

void ChaosEditor::RigidBodyComponentGUI::XRotationConstraintUIChanged(bool p_x)
{
	if (m_rigidbody)
	m_rigidbody->SetXRotationConstraint(p_x);
}

void ChaosEditor::RigidBodyComponentGUI::YRotationConstraintUIChanged(bool p_y)
{
	if (m_rigidbody)
	m_rigidbody->SetYRotationConstraint(p_y);
}

void ChaosEditor::RigidBodyComponentGUI::ZRotationConstraintUIChanged(bool p_z)
{
	if (m_rigidbody)
	m_rigidbody->SetZRotationConstraint(p_z);
}


void ChaosEditor::RigidBodyComponentGUI::XTranslationConstraintUIChanged(bool p_x)
{
	if (m_rigidbody)
	m_rigidbody->SetXTranslationConstraint(p_x);
}

void ChaosEditor::RigidBodyComponentGUI::YTranslationConstraintUIChanged(bool p_y)
{
	if (m_rigidbody)
	m_rigidbody->SetYTranslationConstraint(p_y);
}

void ChaosEditor::RigidBodyComponentGUI::ZTranslationConstraintUIChanged(bool p_z)
{
	if (m_rigidbody)
	m_rigidbody->SetZTranslationConstraint(p_z);
}


void ChaosEditor::RigidBodyComponentGUI::CollisionDetectionModeUIChanged(const QString& p_string)
{
	if (p_string == "CCD")
	{
		m_rigidbody->Actor().EnableCCDCollisionDetection();
	}
	else
	{
		m_rigidbody->Actor().EnableDiscretCollisionDetection();
	}
}

QGroupBox * ChaosEditor::RigidBodyComponentGUI::CreateRotationConstraints()
{
	QGroupBox* RotationConstraints = new QGroupBox();
	RotationConstraints->setTitle("");
	QHBoxLayout* layout = new QHBoxLayout();
	QCheckBox* xRot = new QCheckBox("X");
	QCheckBox* yRot = new QCheckBox("Y");
	QCheckBox* zRot = new QCheckBox("Z");
	QObject::connect(xRot, &QCheckBox::stateChanged, this, &RigidBodyComponentGUI::XRotationConstraintUIChanged);
	QObject::connect(yRot, &QCheckBox::stateChanged, this, &RigidBodyComponentGUI::YRotationConstraintUIChanged);
	QObject::connect(zRot, &QCheckBox::stateChanged, this, &RigidBodyComponentGUI::ZRotationConstraintUIChanged);
	if (m_rigidbody->GetXRotationConstraint())
		xRot->setChecked(true);

	if (m_rigidbody->GetYRotationConstraint())
		yRot->setChecked(true);

	if (m_rigidbody->GetZRotationConstraint())
		zRot->setChecked(true);

	layout->addWidget(xRot);
	layout->addWidget(yRot);
	layout->addWidget(zRot);
	layout->addStretch(1);
	RotationConstraints->setLayout(layout);
	return RotationConstraints;
}

QGroupBox * ChaosEditor::RigidBodyComponentGUI::CreateTranslationConstraints()
{
	QGroupBox* TranslationConstraints = new QGroupBox();
	TranslationConstraints->setTitle("");
	QHBoxLayout* layout = new QHBoxLayout();
	QCheckBox* xTr = new QCheckBox("X");
	QCheckBox* yTr = new QCheckBox("Y");
	QCheckBox* zTr = new QCheckBox("Z");
	QObject::connect(xTr, &QCheckBox::stateChanged, this, &RigidBodyComponentGUI::XTranslationConstraintUIChanged);
	QObject::connect(yTr, &QCheckBox::stateChanged, this, &RigidBodyComponentGUI::YTranslationConstraintUIChanged);
	QObject::connect(zTr, &QCheckBox::stateChanged, this, &RigidBodyComponentGUI::ZTranslationConstraintUIChanged);
	if (m_rigidbody->GetXTranslationConstraint())
		xTr->setChecked(true);

	if (m_rigidbody->GetYTranslationConstraint())
		yTr->setChecked(true);

	if (m_rigidbody->GetZTranslationConstraint())
		zTr->setChecked(true);

	layout->addWidget(xTr);
	layout->addWidget(yTr);
	layout->addWidget(zTr);
	layout->addStretch(1);
	TranslationConstraints->setLayout(layout);
	return TranslationConstraints;
}

QComboBox * ChaosEditor::RigidBodyComponentGUI::CreateCollisionDetectionMode()
{
	QComboBox* comboBox = new QComboBox();
	comboBox->addItem("Discrete");
	comboBox->addItem("CCD");
	QObject::connect(comboBox, &QComboBox::currentTextChanged, this, &RigidBodyComponentGUI::CollisionDetectionModeUIChanged);
	physx::PxRigidBodyFlags flag = static_cast<physx::PxRigidBody*>(&m_rigidbody->Actor().PhysicsActor())->getRigidBodyFlags();
	if (flag.isSet(physx::PxRigidBodyFlag::eENABLE_CCD))
		comboBox->setCurrentText("CCD");

	else
		comboBox->setCurrentText("Discrete");

	return comboBox;
}