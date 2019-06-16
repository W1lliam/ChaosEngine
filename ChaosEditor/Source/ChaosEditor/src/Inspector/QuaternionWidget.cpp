#include "ChaosEditor/Inspector/QuaternionWidget.h"
#include <QHBoxLayout>

namespace ChaosEditor
{
    QuaternionWidget::QuaternionWidget(bool p_showAsVector3, QWidget * p_parent, Qt::WindowFlags p_flags) :
        QWidget(p_parent, p_flags),
        x(new DragSpinBox("X", this)), 
        y(new DragSpinBox("Y", this)), 
        z(new DragSpinBox("Z", this)),
        w(p_showAsVector3 ? nullptr : new DragSpinBox("W", this)),
        m_asVector3(p_showAsVector3)
    {
        QHBoxLayout* layout = new QHBoxLayout;

        layout->setSpacing(0);
        layout->setMargin(0);

        x->GetLabel()->setMinimumWidth(10);
        y->GetLabel()->setMinimumWidth(10);
        z->GetLabel()->setMinimumWidth(10);

        layout->addWidget(x);
        layout->addWidget(y);
        layout->addWidget(z);

        if (!m_asVector3)
        {
            w->GetLabel()->setMinimumWidth(10);
            layout->addWidget(w);
        }
        setLayout(layout);

        connect(x, SIGNAL(ValueChanged(double)), this, SIGNAL(XValueChanged(double)));
        connect(y, SIGNAL(ValueChanged(double)), this, SIGNAL(YValueChanged(double)));
        connect(z, SIGNAL(ValueChanged(double)), this, SIGNAL(ZValueChanged(double)));
        if (!m_asVector3)
            connect(w, SIGNAL(ValueChanged(double)), this, SIGNAL(WValueChanged(double)));

        connect(x, SIGNAL(ValueChanged(double)), this, SLOT(AnyValueChanged(double)));
        connect(y, SIGNAL(ValueChanged(double)), this, SLOT(AnyValueChanged(double)));
        connect(z, SIGNAL(ValueChanged(double)), this, SLOT(AnyValueChanged(double)));
        if (!m_asVector3)
            connect(w, SIGNAL(ValueChanged(double)), this, SLOT(AnyValueChanged(double)));

    }

    void QuaternionWidget::SetValue(double p_x, double p_y, double p_z, double p_w)
    {
        if (!m_asVector3)
        {
            x->SetValue(p_x);
            y->SetValue(p_y);
            z->SetValue(p_z);
            w->SetValue(p_z);
        }
        else
        {
            Chaos::Maths::Quaternion quat(p_x, p_y, p_z, p_w);
            m_eulerAngles = quat.ToEulerAngles();
            x->SetValue(m_eulerAngles.x);
            y->SetValue(m_eulerAngles.y);
            z->SetValue(m_eulerAngles.z);
        }
    }

    void QuaternionWidget::SetLinkedVariable(Chaos::Maths::Quaternion* p_variable)
    {
        m_linkedVariable = p_variable;
        if (m_linkedVariable)
        {
            if (!m_asVector3)
            {
                x->SetLinkedVariable(&m_linkedVariable->GetX());
                y->SetLinkedVariable(&m_linkedVariable->GetY());
                z->SetLinkedVariable(&m_linkedVariable->GetZ());
                w->SetLinkedVariable(&m_linkedVariable->GetRealValue());
            }
            else
            {
                x->SetLinkedVariable(&m_eulerAngles.x);
                y->SetLinkedVariable(&m_eulerAngles.y);
                z->SetLinkedVariable(&m_eulerAngles.z);
            }
        }
    }

    void QuaternionWidget::paintEvent(QPaintEvent* event)
    {
        if (m_linkedVariable)
        {
            m_eulerAngles = m_linkedVariable->ToEulerAngles();
        }
        QWidget::paintEvent(event);
    }

    void QuaternionWidget::AnyValueChanged(double p_value)
    {
        if (!m_asVector3)
        {
            emit ValueChanged(x->GetValue(), y->GetValue(), z->GetValue(), w->GetValue());
        }
        else
        {
            Chaos::Maths::Quaternion quat;
            quat.MakeFromEulerAngles(x->GetValue(), y->GetValue(), z->GetValue(), 2, 1, 3);
            emit ValueChanged(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetRealValue());
        }
    }
}