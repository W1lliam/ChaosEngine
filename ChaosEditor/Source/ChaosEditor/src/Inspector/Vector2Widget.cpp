#include "ChaosEditor/Inspector/Vector2Widget.h"
#include <QHBoxLayout>

namespace ChaosEditor
{
	Vector2Widget::Vector2Widget(QWidget * p_parent, Qt::WindowFlags p_flags) :
		QWidget(p_parent, p_flags),
		x("X", this),
		y("Y", this)
	{
		QHBoxLayout* layout = new QHBoxLayout;

		layout->setSpacing(0);
		layout->setMargin(0);

		x.GetLabel()->setMinimumWidth(10);
		y.GetLabel()->setMinimumWidth(10);
		layout->addWidget(&x);
		layout->addWidget(&y);
		setLayout(layout);

		connect(&x, SIGNAL(ValueChanged(double)), this, SIGNAL(XValueChanged(double)));
		connect(&y, SIGNAL(ValueChanged(double)), this, SIGNAL(YValueChanged(double)));

		connect(&x, SIGNAL(ValueChanged(double)), this, SLOT(AnyValueChanged(double)));
		connect(&y, SIGNAL(ValueChanged(double)), this, SLOT(AnyValueChanged(double)));
	}

	void Vector2Widget::SetValue(double p_x, double p_y)
	{
		x.SetValue(p_x);
		y.SetValue(p_y);
	}

	void Vector2Widget::SetLinkedVariable(Chaos::Maths::Vector2 * p_variable)
	{
		m_linkedVariable = p_variable;
		if (m_linkedVariable)
		{
			x.SetLinkedVariable(&m_linkedVariable->x);
			y.SetLinkedVariable(&m_linkedVariable->y);
		}
	}

	Chaos::Maths::Vector2 Vector2Widget::GetValue()
	{
		return Chaos::Maths::Vector2(x.GetValue(), y.GetValue());
	}

	void Vector2Widget::AnyValueChanged(double p_value)
	{
		emit ValueChanged(x.GetValue(), y.GetValue());
	}
}