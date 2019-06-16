#pragma once
#include <QWidget>
#include <ChaosMaths.h>
#include "ChaosEditor/Inspector/DragSpinBox.h"

namespace ChaosEditor
{
    class Vector3Widget : public QWidget
    {
        Q_OBJECT
    public:
        Vector3Widget(QWidget* p_parent = nullptr, Qt::WindowFlags p_flags = Qt::WindowFlags());

        void SetValue(double p_x, double p_y, double p_z);
        void SetLinkedVariable(Chaos::Maths::Vector3* p_variable);

		Chaos::Maths::Vector3 GetValue();

    signals:
        void ValueChanged(double p_x, double p_y, double p_z);
        void XValueChanged(double p_value);
        void YValueChanged(double p_value);
        void ZValueChanged(double p_value);

    public slots:
        void AnyValueChanged(double p_value);

    private:
        DragSpinBox x;
        DragSpinBox y;
        DragSpinBox z;

        Chaos::Maths::Vector3* m_linkedVariable {nullptr};
    };
}