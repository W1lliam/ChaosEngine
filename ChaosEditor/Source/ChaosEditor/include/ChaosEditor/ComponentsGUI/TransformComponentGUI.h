#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"

namespace ChaosEditor
{
    class Vector3Widget;
    class QuaternionWidget;

    class TransformComponentGUI : public AComponentGUI
    {
        Q_OBJECT
    public:
        TransformComponentGUI(QWidget* p_parent = nullptr);
        ~TransformComponentGUI() = default;

        virtual void SetupFrame() override;
        virtual void BindComponent(Chaos::IComponent* p_component) override;

    public slots:
        void PositionUIChanged(double p_x, double p_y, double p_z);
        void RotationUIChanged(double p_x, double p_y, double p_z, double p_w);
        void ScaleUIChanged(double p_x, double p_y, double p_z);

    protected:
        Chaos::TransformComponent*  m_component     {nullptr};
        Vector3Widget*              m_position      {nullptr};
        QuaternionWidget*           m_rotation      {nullptr};
        Vector3Widget*              m_scale         {nullptr};
    };
}