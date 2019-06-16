#include "ChaosEditor/ComponentsGUI/LightComponentGUI.h"
#include "ChaosEditor/Inspector/DragSpinBox.h"
#include "ChaosEditor/Inspector/RangeEdit.h"
#include "ChaosEditor/Inspector/ColorPicker.h"
#include <QFrame>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>

namespace ChaosEditor
{
    LightComponentGUI::LightComponentGUI(QWidget* p_parent) : 
        AComponentGUI("Light", p_parent),
        m_layout(new QGridLayout),

        m_type(new QComboBox(this)),
        m_color(new ColorPicker(QColor(50,100,10), this)),
        m_rangeLabel(new DragLabel("Range", this)),
        m_range(new DragSpinBox(m_rangeLabel, this)),
        m_intensityLabel(new DragLabel("Intensity", this)),
        m_intensity(new DragSpinBox(m_intensityLabel, this)),
        m_angle(new RangeEdit(this)),
        m_falloffLabel(new DragLabel("Falloff", this)),
        m_falloff(new DragSpinBox(m_falloffLabel, this)),
        m_castShadows(new QCheckBox(this))
    {
        m_type->addItem("Directional",  (uint8_t)Chaos::E_LightType::DIRECTIONAL);
        m_type->addItem("Point",        (uint8_t)Chaos::E_LightType::SPOT		);
        m_type->addItem("Spot",         (uint8_t)Chaos::E_LightType::SPOT		);

        m_angle->SetMultiplier(2);
        m_angle->SetMinValue(1);
        m_angle->SetMaxValue(180);

        m_intensity->SetMinValue(0);
        m_intensity->SetMultiplier(10);

        m_range->SetMinValue(0);

        m_rangeLabel->setHidden(true);
        m_range->setHidden(true);
        m_angle->setHidden(true);
        m_falloffLabel->setHidden(true);
        m_falloff->setHidden(true);

        m_layout->setSpacing(0);
        
        m_layout->addWidget(new QLabel("Type"),         0, 0, Qt::AlignLeft);
        m_layout->addWidget(m_type,                     0, 1, Qt::AlignRight);

        m_layout->addWidget(new QLabel("Color"),        1, 0, Qt::AlignLeft);
        m_layout->addWidget(m_color,                    1, 1, Qt::AlignRight);

        m_layout->addWidget(m_intensityLabel,           2, 0, Qt::AlignLeft);
        m_layout->addWidget(m_intensity,                2, 1, Qt::AlignRight);

        m_layout->addWidget(new QLabel("Cast Shadows"), 3, 0, Qt::AlignLeft);
        m_layout->addWidget(m_castShadows,              3, 1, Qt::AlignRight);

        m_layout->setColumnStretch(0, 0);
        m_layout->setColumnStretch(1, 5);

        m_layout->setRowMinimumHeight(0, 20);
        m_layout->setRowMinimumHeight(1, 30);
        m_layout->setRowMinimumHeight(2, 20);
        m_layout->setRowMinimumHeight(3, 20);
        m_layout->setRowMinimumHeight(4, 20);
        m_layout->setRowMinimumHeight(5, 60);
        m_layout->setRowMinimumHeight(6, 20);

        QObject::connect(m_type, SIGNAL(currentIndexChanged(int)),
            this, SLOT(TypeChanged(int)));

        QObject::connect(m_color, SIGNAL(ColorChanged(int, int, int, int)),
            this, SLOT(ColorChanged(int, int, int, int)));

        QObject::connect(m_range, SIGNAL(ValueChanged(double)),
            this, SLOT(RangeChanged(double)));

        QObject::connect(m_intensity, SIGNAL(ValueChanged(double)),
            this, SLOT(IntensityChanged(double)));

        QObject::connect(m_angle, SIGNAL(ValueChanged(double)),
            this, SLOT(AngleChanged(double)));

        QObject::connect(m_falloff, SIGNAL(ValueChanged(double)),
            this, SLOT(FallOffChanged(double)));

        QObject::connect(m_castShadows, &QCheckBox::stateChanged,
            this, &LightComponentGUI::CastShadowsChanged);
    }

    void LightComponentGUI::SetupFrame()
    {
        CHAOS_ASSERT(m_component, "Drawing non bound ComponentGUI");

        if (m_layout->itemAtPosition(4, 0))
        {
            m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(4, 0)))->widget()->setHidden(true);
            m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(4, 1)))->widget()->setHidden(true);
        }

        if (m_layout->itemAtPosition(5, 0))
        {
            m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(5, 0)))->widget()->deleteLater();
            m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(5, 1)))->widget()->setHidden(true);

            m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(6, 0)))->widget()->setHidden(true);
            m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(6, 1)))->widget()->setHidden(true);
        }

        if (m_component->m_type == Chaos::E_LightType::SPOT)
        {
            m_layout->addWidget(m_rangeLabel,               4, 0, Qt::AlignLeft);
            m_layout->addWidget(m_range,                    4, 1, Qt::AlignRight);
            m_rangeLabel->setVisible(true);
            m_range->setVisible(true);

            if (m_type->currentIndex() >= 2 || m_component->GetAngle() < 180.0f) // SPOT
            {
                m_layout->setSpacing(1);
                m_layout->addWidget(new QLabel("Angle"),    5, 0, Qt::AlignLeft);
                m_layout->addWidget(m_angle,                5, 1, Qt::AlignRight);
                m_angle->setVisible(true);

                m_layout->addWidget(m_falloffLabel,         6, 0, Qt::AlignLeft);
                m_layout->addWidget(m_falloff,              6, 1, Qt::AlignRight);
                m_falloffLabel->setVisible(true);
                m_falloff->setVisible(true);
            }
        }
       
        setLayout(m_layout);
    }

    void LightComponentGUI::BindComponent(Chaos::IComponent* p_component)
    {
        Chaos::LightComponent* lightComponent = dynamic_cast<Chaos::LightComponent*>(p_component);
        CHAOS_ASSERT(lightComponent, "Wrong component type!");
        m_component = lightComponent;

		if ((int)m_component->m_type >= 2 || m_component->GetAngle() < 180.0f) // SPOT
		{
			m_type->setCurrentIndex(2);
		}
		else
		{
			m_type->setCurrentIndex((int)m_component->m_type);
		}

        m_color->SetColor(QColor(m_component->m_color.x * 255, m_component->m_color.y * 255, m_component->m_color.z * 255));
        m_range->SetLinkedVariable(&m_component->m_range);
        m_intensity->SetLinkedVariable(&m_component->m_intensity);
        m_falloff->SetLinkedVariable(&m_component->m_falloff);
        m_angle->SetLinkedVariable(&m_component->m_angle);
        m_castShadows->setChecked(m_component->m_castShadows);
    }

    void LightComponentGUI::TypeChanged(int p_index)
    {
        if (m_component)
        {
            m_component->SetType(static_cast<Chaos::E_LightType>(m_type->currentData().toUInt()));
			if (p_index == 1)
			{
				m_component->SetAngle(180.0);
			}
            SetupFrame();
        }
    }

    void LightComponentGUI::ColorChanged(int p_red, int p_green, int p_blue, int p_alpha)
    {
        if (m_component)
        {
            m_component->SetColor({ p_red / 255.0f, p_green / 255.0f, p_blue / 255.0f, 1.0f });
        }
    }
    void LightComponentGUI::RangeChanged(double p_value)
    {
        if (m_component)
        {
            m_component->SetRange(p_value);
        }
    }
    void LightComponentGUI::IntensityChanged(double p_value)
    {
        if (m_component)
        {
            m_component->SetIntensity(p_value);
        }
    }
    void LightComponentGUI::AngleChanged(double p_value)
    {
        if (m_component)
        {
            m_component->SetAngle(p_value);
        }
    }
    void LightComponentGUI::FallOffChanged(double p_value)
    {
        if (m_component)
        {
            m_component->SetFalloff(p_value);
        }
    }
    void LightComponentGUI::CastShadowsChanged(bool p_enabled)
    {
        if (m_component)
        {
            m_component->SetCastShadows(p_enabled);
        }
    }
}