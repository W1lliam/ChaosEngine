#include "ChaosEditor/Inspector/Vector3Widget.h"
#include <QHBoxLayout>

namespace ChaosEditor
{
    Vector3Widget::Vector3Widget(QWidget * p_parent, Qt::WindowFlags p_flags) :
        QWidget(p_parent, p_flags),
        x("X", this), 
        y("Y", this), 
        z("Z", this)
    {
        QHBoxLayout* layout = new QHBoxLayout;

        layout->setSpacing(0);
        layout->setMargin(0);

        x.GetLabel()->setMinimumWidth(10);
        y.GetLabel()->setMinimumWidth(10);
        z.GetLabel()->setMinimumWidth(10);
        layout->addWidget(&x);
        layout->addWidget(&y);
        layout->addWidget(&z);
        setLayout(layout);

        connect(&x, SIGNAL(ValueChanged(double)), this, SIGNAL(XValueChanged(double)));
        connect(&y, SIGNAL(ValueChanged(double)), this, SIGNAL(YValueChanged(double)));
        connect(&z, SIGNAL(ValueChanged(double)), this, SIGNAL(ZValueChanged(double)));

        connect(&x, SIGNAL(ValueChanged(double)), this, SLOT(AnyValueChanged(double)));
        connect(&y, SIGNAL(ValueChanged(double)), this, SLOT(AnyValueChanged(double)));
        connect(&z, SIGNAL(ValueChanged(double)), this, SLOT(AnyValueChanged(double)));
    }

    void Vector3Widget::SetValue(double p_x, double p_y, double p_z)
    {
        x.SetValue(p_x);
        y.SetValue(p_y);
        z.SetValue(p_z);
    }

    void Vector3Widget::SetLinkedVariable(Chaos::Maths::Vector3 * p_variable)
    {
        m_linkedVariable = p_variable;
        if (m_linkedVariable)
        {
            x.SetLinkedVariable(&m_linkedVariable->x);
            y.SetLinkedVariable(&m_linkedVariable->y);
            z.SetLinkedVariable(&m_linkedVariable->z);
        }
    }

	Chaos::Maths::Vector3 Vector3Widget::GetValue()
	{
		return Chaos::Maths::Vector3(x.GetValue(), y.GetValue(), z.GetValue());
	}

    void Vector3Widget::AnyValueChanged(double p_value)
    {
        emit ValueChanged(x.GetValue(), y.GetValue(), z.GetValue());
    }
}