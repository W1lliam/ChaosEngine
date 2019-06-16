#pragma once
#include <QWidget>
#include <ChaosMaths.h>
#include "ChaosEditor/Inspector/DragSpinBox.h"

namespace ChaosEditor
{
    class QuaternionWidget : public QWidget
    {
        Q_OBJECT
    public:
        QuaternionWidget(bool p_showAsVector3 = true, QWidget* p_parent = nullptr, Qt::WindowFlags p_flags = Qt::WindowFlags());

        void SetValue(double p_x, double p_y, double p_z, double p_w);
        void SetLinkedVariable(Chaos::Maths::Quaternion* p_variable);

    signals:
        void ValueChanged(double p_x, double p_y, double p_z, double p_w);
        void XValueChanged(double p_value);
        void YValueChanged(double p_value);
        void ZValueChanged(double p_value);
        void WValueChanged(double p_value);

    public slots:
        void AnyValueChanged(double p_value);

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        DragSpinBox* x;
        DragSpinBox* y;
        DragSpinBox* z;
        DragSpinBox* w;
        bool m_asVector3 {true};

        Chaos::Maths::Quaternion*   m_linkedVariable {nullptr};
        Chaos::Maths::Vector3       m_eulerAngles;
    };
}