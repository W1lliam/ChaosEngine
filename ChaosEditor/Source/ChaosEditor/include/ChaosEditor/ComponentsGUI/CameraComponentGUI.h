#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"

class QComboBox;
class QGridLayout;

namespace ChaosEditor
{
    class DragSpinBox;
    class RangeEdit;
    class DragLabel;

    class CameraComponentGUI : public AComponentGUI
    {
        Q_OBJECT
    public:
        CameraComponentGUI(QWidget* p_parent = nullptr);
        ~CameraComponentGUI() = default;

        virtual void SetupFrame() override;
        virtual void BindComponent(Chaos::IComponent* p_component) override;

    public slots:
        void ProjectionChanged(int p_index);
        void ProjectionNearChanged(double p_value);
        void ProjectionFarChanged(double p_value);
        void ProjectionFOVChanged(double p_value);
        void ProjectionScaleChanged(double p_value);

    protected:
        void SetupPerspectiveUI();
        void SetupOrtographicUI();
    protected:
        Chaos::CameraComponent* m_component     {nullptr};
        QGridLayout*            m_layout        {nullptr};

        QComboBox*              m_projection    {nullptr};
        DragSpinBox*            m_near          {nullptr};
        DragSpinBox*            m_far           {nullptr};
        RangeEdit*              m_fov           {nullptr};

        DragLabel*              m_orthoSizeLabel{nullptr};
        DragSpinBox*            m_orthoSize     {nullptr};
    };
}