#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"

class QColorDialog;
class QPushButton;
class QComboBox;
class QGridLayout;
class QCheckBox;

namespace ChaosEditor
{
    class DragSpinBox;
    class ColorPicker;
    class DragLabel;
    class RangeEdit;

    class LightComponentGUI : public AComponentGUI
    {
        Q_OBJECT
    public:
        LightComponentGUI(QWidget* p_parent = nullptr);
        ~LightComponentGUI() = default;

        virtual void SetupFrame() override;
        virtual void BindComponent(Chaos::IComponent* p_component) override;

    protected slots:
        void TypeChanged(int p_index);
        void ColorChanged(int p_red, int p_green, int p_blue, int p_alpha);
        void RangeChanged(double p_value);
        void IntensityChanged(double p_value);
        void AngleChanged(double p_value);
        void FallOffChanged(double p_value);
        void CastShadowsChanged(bool p_enabled);
        

    protected:

        Chaos::LightComponent*  m_component         {nullptr};
        QGridLayout*            m_layout            {nullptr};

        QComboBox*	            m_type              {nullptr};
        QCheckBox*			    m_castShadows       {nullptr};
        DragLabel*			    m_rangeLabel        {nullptr};
        DragSpinBox*			m_range             {nullptr};
        ColorPicker*            m_color             {nullptr};
        DragLabel*			    m_intensityLabel    {nullptr};
        DragSpinBox*			m_intensity         {nullptr};
        RangeEdit*			    m_angle             {nullptr};
        DragLabel*			    m_falloffLabel      {nullptr};
        DragSpinBox*			m_falloff           {nullptr};
    };
}