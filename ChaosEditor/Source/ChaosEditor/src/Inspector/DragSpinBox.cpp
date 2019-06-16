#include "ChaosEditor/Inspector/DragSpinBox.h"
#include <ChaosDebug.h>
#include <chrono>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QDoubleValidator>
#include <QHBoxLayout>

namespace ChaosEditor
{
    DragSpinBox::DragSpinBox(const QString& p_label,QWidget* p_parent) : 
        QWidget(p_parent), 
        m_label(new DragLabel(p_label)), 
        m_field(new QLineEdit("0.000"))
    {
        Setup();
    }
    DragSpinBox::DragSpinBox(DragLabel* p_label, QWidget* p_parent) :
        QWidget(p_parent), 
        m_label(p_label),
        m_field(new QLineEdit("0.000"))
    {
        Setup();
    }

    void DragSpinBox::DragStarted(QMouseEvent* p_event)
    {
        m_mouseStartPosX = QCursor::pos().x();
    }

    void DragSpinBox::Dragged(QMouseEvent* p_event)
    {
        QRect rec = QApplication::desktop()->screenGeometry();

        double valueOffset = (QCursor::pos().x() - m_mouseStartPosX) / rec.width() * m_multiplier;
        double newValue = m_field->text().toDouble() + valueOffset;
        if (newValue <= m_validator->top() && newValue > m_validator->bottom())
        {
            m_field->setText(QString::number(newValue, 'g', 4));
        }

        emit ValueChanged(m_field->text().toDouble());

        if (QCursor::pos().x() >= rec.width() - 1)
        {
            cursor().setPos(1, QCursor::pos().y());
        }
        if (QCursor::pos().x() <= 0)
        {
            cursor().setPos(QCursor::pos().x() + rec.width(), QCursor::pos().y());
        }
        m_mouseStartPosX = QCursor::pos().x();
    }

    void DragSpinBox::DragStopped(QMouseEvent* p_event)
    {
        emit ValueChanged(m_field->text().toDouble());
    }

    void DragSpinBox::FieldChanged(const QString & text)
    {
        emit ValueChanged(text.toDouble());
    }

    void DragSpinBox::Setup()
    {
        QLocale loc(QLocale::C);
        loc.setNumberOptions(QLocale::RejectGroupSeparator);

        m_validator = new QDoubleValidator(this);
        m_validator->setLocale(loc);
        m_validator->setDecimals(4);
        m_field->setValidator(m_validator);
        m_field->setMinimumWidth(40);
        m_field->setAlignment(Qt::AlignRight);
        m_field->installEventFilter(this);

        QHBoxLayout* layout = new QHBoxLayout;
        layout->setSpacing(5);
        layout->setMargin(3);

        layout->addWidget(m_label, 0, Qt::AlignRight);
        layout->addWidget(m_field, 1, Qt::AlignLeft);
        setLayout(layout);

        connect(m_label, SIGNAL(DragStart(QMouseEvent*)), this, SLOT(DragStarted(QMouseEvent*)));
        connect(m_label, SIGNAL(Drag(QMouseEvent*)), this, SLOT(Dragged(QMouseEvent*)));
        connect(m_label, SIGNAL(DragStop(QMouseEvent*)), this, SLOT(DragStopped(QMouseEvent*)));

    }

    bool DragSpinBox::eventFilter(QObject * obj, QEvent * event)
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

    void DragSpinBox::paintEvent(QPaintEvent* event)
    {
			if (m_linkedVariable && *m_linkedVariable != m_field->text().toDouble() && !m_field->hasFocus())
			{
				m_field->setText(QString::number(*m_linkedVariable, 'g', 4));
			}

    }
}