#pragma once
#include <QLineEdit>
#include <QSlider>
#include <QDoubleValidator>

namespace ChaosEditor
{
    class RangeEdit : public QWidget
    {
        Q_OBJECT
    public:
        RangeEdit(QWidget* p_parent = nullptr, Qt::Orientation p_orientation = Qt::Horizontal);

        inline void         SetValue(double p_value)    {   
                                                            m_slider->setValue(p_value * m_multiplier);
                                                            m_field->setText(QString::number(p_value, 'g', 4));
                                                        };
        inline void         SetMinValue(double p_value)                 { m_validator->setBottom(p_value); m_slider->setMinimum(p_value * m_multiplier); }
        inline void         SetMaxValue(double p_value)                 { m_validator->setTop(p_value); m_slider->setMaximum(p_value * m_multiplier); }
        inline void         SetMultiplier(float p_value)                { m_multiplier = p_value; }
        inline void         SetLinkedVariable(const float* p_variable)  { m_linkedVariable = p_variable; }
        inline double       GetValue() const                            { return m_field->text().toDouble(); }


    signals:
        void ValueChanged(double p_value);

    public slots:
        void FieldChanged(const QString &text);
        void RangeChanged(int p_value);

    protected:
        bool eventFilter(QObject* obj, QEvent *event);
        void paintEvent(QPaintEvent *event) override;

    protected:
        QLineEdit*          m_field;
        QSlider*            m_slider;
        QDoubleValidator*   m_validator;
        float               m_multiplier         {1.0f};
        const float*        m_linkedVariable    {nullptr};
    };
}