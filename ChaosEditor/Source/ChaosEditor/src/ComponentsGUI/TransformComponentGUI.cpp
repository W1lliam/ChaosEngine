#include "ChaosEditor/ComponentsGUI/TransformComponentGUI.h"
#include "ChaosEditor/Inspector/Vector3Widget.h"
#include "ChaosEditor/Inspector/QuaternionWidget.h"
#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>
#include <QLabel>

namespace ChaosEditor
{
    TransformComponentGUI::TransformComponentGUI(QWidget* p_parent) : AComponentGUI("Transform", p_parent),
        m_position(new Vector3Widget),
        m_rotation(new QuaternionWidget),
        m_scale(new Vector3Widget)
    {
        QObject::connect(   m_position, &Vector3Widget::ValueChanged,
                            this,       &TransformComponentGUI::PositionUIChanged);
        QObject::connect(   m_rotation, &QuaternionWidget::ValueChanged,
                            this,       &TransformComponentGUI::RotationUIChanged);
        QObject::connect(   m_scale,    &Vector3Widget::ValueChanged,
                            this,       &TransformComponentGUI::ScaleUIChanged);
    }

    void TransformComponentGUI::SetupFrame()
    {
        QGridLayout* contentLayout = new QGridLayout;

        contentLayout->setSpacing(0);

        contentLayout->addWidget(new QLabel("Position"),    0, 0, Qt::AlignLeft);
        contentLayout->addWidget(m_position,                0, 1, Qt::AlignRight);

        contentLayout->addWidget(new QLabel("Rotation"),    1, 0, Qt::AlignLeft);
        contentLayout->addWidget(m_rotation,                1, 1, Qt::AlignRight);

        contentLayout->addWidget(new QLabel("Scale"),       2, 0, Qt::AlignLeft);
        contentLayout->addWidget(m_scale,                   2, 1, Qt::AlignRight);

        contentLayout->setColumnStretch(0, 1);
        contentLayout->setColumnStretch(1, 2);
        setLayout(contentLayout);
    }

    void TransformComponentGUI::BindComponent(Chaos::IComponent* p_component)
    {
        Chaos::TransformComponent* transformComponent = dynamic_cast<Chaos::TransformComponent*>(p_component);
        CHAOS_ASSERT(transformComponent, "Wrong component type!");

        m_position->SetLinkedVariable(&transformComponent->GetPositionRef());
        m_rotation->SetLinkedVariable(&transformComponent->GetRotationRef());
        m_scale->SetLinkedVariable(&transformComponent->GetScaleRef());

        m_component = transformComponent;

    }

    void TransformComponentGUI::PositionUIChanged(double p_x, double p_y, double p_z)
    {
        if (m_component)
        {
            m_component->AsTransform().SetPosition(p_x, p_y, p_z);
        }
    }
    void TransformComponentGUI::RotationUIChanged(double p_x, double p_y, double p_z, double p_w)
    {
        if (m_component)
        {
            m_component->AsTransform().SetOrientation(p_x, p_y, p_z, (float)p_w);
        }
    }
    void TransformComponentGUI::ScaleUIChanged(double p_x, double p_y, double p_z)
    {
        if (m_component)
        {
            m_component->AsTransform().SetScale(p_x, p_y, p_z);
        }
    }

}