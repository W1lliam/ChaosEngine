#pragma once
#include <QWidget>
#include <ChaosMaths.h>
#include "ChaosEditor/Inspector/DragSpinBox.h"

namespace ChaosEditor
{
	class Vector2Widget : public QWidget
	{
		Q_OBJECT
	public:
		Vector2Widget(QWidget* p_parent = nullptr, Qt::WindowFlags p_flags = Qt::WindowFlags());

		void SetValue(double p_x, double p_y);
		void SetLinkedVariable(Chaos::Maths::Vector2* p_variable);

		Chaos::Maths::Vector2 GetValue();

	signals:
		void ValueChanged(double p_x, double p_y);
		void XValueChanged(double p_value);
		void YValueChanged(double p_value);

	public slots:
		void AnyValueChanged(double p_value);

	private:
		DragSpinBox x;
		DragSpinBox y;

		Chaos::Maths::Vector2* m_linkedVariable{ nullptr };
	};
}