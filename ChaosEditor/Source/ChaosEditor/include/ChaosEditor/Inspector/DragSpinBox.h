#pragma once
#include <QLineEdit>
#include <QLabel>
#include <QDoubleValidator>
#include "ChaosEditor/Inspector/DragLabel.h"

namespace ChaosEditor
{
    class DragSpinBox : public QWidget
    {
        Q_OBJECT
    public:
        DragSpinBox(const QString& p_label, QWidget* p_parent = nullptr);
        DragSpinBox(DragLabel* p_label,     QWidget* p_parent = nullptr);

        inline void         SetMultiplier(double p_multiplier)          { m_multiplier = p_multiplier; }
        inline void         SetValue(double p_value)                    { m_field->setText(QString::number(p_value, 'g', 4)); }
        inline void         SetMinValue(double p_value)                 { m_validator->setBottom(p_value); }
        inline void         SetMaxValue(double p_value)                 { m_validator->setTop(p_value); }
        inline void         SetLinkedVariable(const float* p_variable)  { m_linkedVariable = p_variable; }
        inline DragLabel*   GetLabel()                                  { return m_label; }
        inline bool			FieldHasFocus()                             { return m_field->hasFocus(); }
        inline double       GetValue() const                            { return m_field->text().toDouble(); }

    signals:
        void ValueChanged(double p_value);

    public slots:
        void DragStarted(QMouseEvent* p_event);
        void Dragged(QMouseEvent* p_event);
        void DragStopped(QMouseEvent* p_event);
        void FieldChanged(const QString &text);

    protected:
        void Setup();
        bool eventFilter(QObject* obj, QEvent *event);
        void paintEvent(QPaintEvent *event) override;

    protected:
        QLineEdit*          m_field;
        DragLabel*          m_label;
        QDoubleValidator*   m_validator;
        double              m_mouseStartPosX    {0};
        double              m_multiplier        {200};
        const float*        m_linkedVariable    {nullptr};
		double				m_refreshRate		{0.5};
		std::chrono::high_resolution_clock::time_point m_lastUpdate;
    };
}