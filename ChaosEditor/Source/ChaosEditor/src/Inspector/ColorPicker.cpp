#include "Chaoseditor/Inspector/ColorPicker.h"
#include <QColorDialog>
#include <QHBoxLayout>

namespace ChaosEditor
{
    ColorPicker::ColorPicker(QColor p_color, QWidget* p_parent) :
        QPushButton(p_parent),
        m_color(p_color),
        m_dialog(new QColorDialog(m_color, this))
    {
        setStyleSheet("width: 100%;");
        setPalette(QPalette(p_color));
        setAutoFillBackground(true);
        m_dialog->setOptions(QColorDialog::ColorDialogOption::DontUseNativeDialog);

        connect(this, SIGNAL(clicked()), this, SLOT(SetColor()));
        connect(m_dialog, SIGNAL(currentColorChanged(const QColor &)), this, SLOT(ColorChanged(const QColor &)));
    }

    void ColorPicker::SetColor(const QColor& p_color)
    {
        m_color = p_color;
        setPalette(QPalette(m_color));
        setAutoFillBackground(true);
        m_dialog->setCurrentColor(m_color);
    }

    void ColorPicker::SetColor()
    {
        m_dialog->open();
    }
    void ColorPicker::ColorChanged(const QColor & color)
    {
        if (m_color.isValid())
        {
            m_color = color;
            setPalette(QPalette(m_color));
            setAutoFillBackground(true);
            m_dialog->setCurrentColor(m_color);
            emit ColorChanged(m_color.red(), m_color.green(), m_color.blue(), m_color.alpha());
        }
    }
}