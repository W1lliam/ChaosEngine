#include "ChaosEditor/ComponentsGUI/CameraComponentGUI.h"
#include "ChaosEditor/Inspector/DragSpinBox.h"
#include "ChaosEditor/Inspector/RangeEdit.h"
#include "ChaosEditor/CameraSystem.h"
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QTreeWidgetItem>

namespace ChaosEditor
{
    CameraComponentGUI::CameraComponentGUI(QWidget* p_parent) :
        AComponentGUI("Camera", p_parent),
        m_layout (new QGridLayout),
        m_projection(new QComboBox(this)),
        m_near(new DragSpinBox("Near", this)),
        m_far(new DragSpinBox("Far", this)),
        m_fov(new RangeEdit(this)),
        m_orthoSizeLabel(new DragLabel("Scale", this)),
        m_orthoSize(new DragSpinBox(m_orthoSizeLabel, this))
    {
        m_fov->SetMinValue(1);
        m_fov->SetMaxValue(180);
        m_fov->setHidden(true);
        m_orthoSize->setHidden(true);

        m_layout->setSpacing(1);
       
        m_projection->addItem("Perspective", (uint8_t)Chaos::E_CameraType::PERSPECTIVE);
        m_projection->addItem("Orthographic", (uint8_t)Chaos::E_CameraType::ORTHOGRAPHIC);

        m_layout->addWidget(new QLabel("Projection"), 0, 0, Qt::AlignLeft);
        m_layout->addWidget(m_projection, 0, 1, Qt::AlignRight);
       
        m_layout->addWidget(new QLabel("Clipping Planes"), 2, 0, Qt::AlignLeft);
        m_layout->addWidget(m_near, 2, 1, Qt::AlignRight);
        m_layout->addWidget(m_far, 3, 1, Qt::AlignRight);

        m_layout->setColumnStretch(0, 1);
        m_layout->setColumnStretch(1, 2);
        m_layout->setRowMinimumHeight(0, 20);
        m_layout->setRowMinimumHeight(1, 40);
        m_layout->setRowMinimumHeight(2, 20);
        m_layout->setRowMinimumHeight(3, 20);


        QObject::connect(m_projection,  SIGNAL(currentIndexChanged(int)),
                         this,          SLOT(ProjectionChanged(int)));

        QObject::connect(m_near,        SIGNAL(ValueChanged(double)),
                         this,          SLOT(ProjectionNearChanged(double)));

        QObject::connect(m_far,         SIGNAL(ValueChanged(double)),
                         this,          SLOT(ProjectionFarChanged(double)));

        QObject::connect(m_fov,         SIGNAL(ValueChanged(double)),
                         this,          SLOT(ProjectionFOVChanged(double)));

        QObject::connect(m_orthoSize,   SIGNAL(ValueChanged(double)),
                         this,          SLOT(ProjectionScaleChanged(double)));
    }

    void CameraComponentGUI::SetupFrame()
    {
        CHAOS_ASSERT(m_component, "Drawing non bound ComponentGUI");
        
        if (m_layout->itemAtPosition(1, 1))
            m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(1, 1)))->widget()->setHidden(true);

        switch (m_component->m_cameraParams.CameraModeSettings.CameraType)
        {
            case Chaos::E_CameraType::PERSPECTIVE:
            {
                if (m_layout->itemAtPosition(1, 0))
                    m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(1, 0)))->widget()->setHidden(true);
                m_fov->setVisible(true);
                m_layout->addWidget(new QLabel("Field of View"), 1, 0, Qt::AlignLeft);
                m_layout->addWidget(m_fov, 1, 1, Qt::AlignRight);
                break;
            }
            case Chaos::E_CameraType::ORTHOGRAPHIC:
            {
                if (m_layout->itemAtPosition(1, 0))
                    m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(1, 0)))->widget()->deleteLater();

                m_orthoSizeLabel->setVisible(true);
                m_orthoSize->setVisible(true);
                m_layout->addWidget(m_orthoSizeLabel,   1, 0, Qt::AlignLeft);
                m_layout->addWidget(m_orthoSize,        1, 1, Qt::AlignRight);
                break;
            }
            default:
                break;
        }

        setLayout(m_layout);
    }

    void CameraComponentGUI::BindComponent(Chaos::IComponent* p_component)
    {
        Chaos::CameraComponent* cameraComponent = dynamic_cast<Chaos::CameraComponent*>(p_component);

        CHAOS_ASSERT(cameraComponent, "Wrong component type!");
        m_component = cameraComponent;

        m_projection->setCurrentIndex((int)m_component->m_cameraParams.CameraModeSettings.CameraType);
        m_near->SetLinkedVariable(&m_component->m_cameraParams.CameraViewParams.Near);
        m_far->SetLinkedVariable(&m_component->m_cameraParams.CameraViewParams.Far);

        m_fov->SetLinkedVariable(&m_component->m_cameraParams.CameraViewParams.FOV);
        m_orthoSize->SetLinkedVariable(&m_component->m_cameraParams.CameraViewParams.OrthographicScale);
    }

    void CameraComponentGUI::ProjectionChanged(int p_index)
    {
        if (m_component)
        {
            m_component->SetType(static_cast<Chaos::E_CameraType>(m_projection->currentData().toUInt()));
            SetupFrame();
            Chaos::SystemManager::Get().GetSystem<Chaos::CameraSystem>()->UpdateProjection();
        }
    }

    void CameraComponentGUI::ProjectionNearChanged(double p_value)
    {
        if (m_component)
        {
            m_component->m_cameraParams.CameraViewParams.Near = p_value;
            Chaos::SystemManager::Get().GetSystem<Chaos::CameraSystem>()->UpdateProjection();
        }
    }

    void CameraComponentGUI::ProjectionFarChanged(double p_value)
    {
        if (m_component)
        {
            m_component->m_cameraParams.CameraViewParams.Far = p_value;
            Chaos::SystemManager::Get().GetSystem<Chaos::CameraSystem>()->UpdateProjection();
        }
    }
    void CameraComponentGUI::ProjectionFOVChanged(double p_value)
    {
        if (m_component)
        {
            m_component->m_cameraParams.CameraViewParams.FOV = p_value;
            Chaos::SystemManager::Get().GetSystem<Chaos::CameraSystem>()->UpdateProjection();
        }
    }
    void CameraComponentGUI::ProjectionScaleChanged(double p_value)
    {
        if (m_component)
        {
            m_component->m_cameraParams.CameraViewParams.OrthographicScale = p_value;
            Chaos::SystemManager::Get().GetSystem<Chaos::CameraSystem>()->UpdateProjection();
        }
    }

    void CameraComponentGUI::SetupPerspectiveUI()
    {
    }

    void CameraComponentGUI::SetupOrtographicUI()
    {
    }
}