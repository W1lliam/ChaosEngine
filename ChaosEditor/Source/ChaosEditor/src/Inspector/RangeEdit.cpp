#include "ChaosEditor/Inspector/RangeEdit.h"
#include <ChaosDebug.h>
#include <QApplication>
#include <QSlider>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QDoubleValidator>
#include <QHBoxLayout>

namespace ChaosEditor
{
    RangeEdit::RangeEdit(QWidget* p_parent, Qt::Orientation p_orientation) :
        QWidget(p_parent),
        m_slider(new QSlider(p_orientation)),
        m_field(new QLineEdit("0.000"))
    {
        QLocale loc(QLocale::C);
        loc.setNumberOptions(QLocale::RejectGroupSeparator);

        m_slider->setFocusPolicy(Qt::StrongFocus);

        m_validator = new QDoubleValidator(this);
        m_validator->setLocale(loc);
        m_validator->setDecimals(4);
        m_field->setValidator(m_validator);
        m_field->setMinimumWidth(40);
        m_field->setAlignment(Qt::AlignRight);
        m_field->installEventFilter(this);

        QHBoxLayout* layout = new QHBoxLayout;

        layout->addWidget(m_slider, 5, Qt::AlignRight);
        layout->addWidget(m_field, 1, Qt::AlignLeft);
        setLayout(layout);

        connect(m_field,    SIGNAL(textEdited(const QString&)), this, SLOT(FieldChanged(const QString&)));
        connect(m_slider,   SIGNAL(valueChanged(int)),          this, SLOT(RangeChanged(int)));
    }

    void RangeEdit::FieldChanged(const QString & text)
    {
        m_slider->setValue(text.toDouble() * m_multiplier);
        emit ValueChanged(text.toDouble());
    }

    void RangeEdit::RangeChanged(int p_value)
    {
        m_field->setText(QString::number(p_value / m_multiplier, 'g', 4));
        emit ValueChanged(p_value / m_multiplier);
    }

    bool RangeEdit::eventFilter(QObject * obj, QEvent * event)
    {
        if (obj == m_field)
        {
            if (event->type() == QEvent::FocusAboutToChange)
            {
                emit ValueChanged(m_field->text().toDouble());
            }
            else if (event->type() == QEvent::KeyPress)
            {
                QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
                if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
                {
                    emit ValueChanged(m_field->text().toDouble());
                    m_field->clearFocus();
                }
            }
        }
        return QObject::eventFilter(obj, event);
    }

    void RangeEdit::paintEvent(QPaintEvent* event)
    {
        if (m_linkedVariable && *m_linkedVariable != m_field->text().toDouble() && !m_field->hasFocus())
        {
            m_field->setText(QString::number(*m_linkedVariable, 'g', 4));
            m_slider->setValue(*m_linkedVariable * m_multiplier);
        }
    }
}