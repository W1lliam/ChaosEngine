#pragma once
#include <ChaosCore.h>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>

namespace ChaosEditor
{
    class ColorPicker : public QPushButton
    {
        Q_OBJECT
    public:
        ColorPicker(QColor p_color = Qt::white, QWidget* p_parent = nullptr);

    signals:
        void ColorChanged(int p_red, int p_green, int p_blue, int p_alpha);

    public slots :
        void SetColor(const QColor& p_color);

    private slots:
        void SetColor();
        void ColorChanged(const QColor &color);

    protected:
        QColor          m_color     {nullptr};
        QColorDialog*   m_dialog    {nullptr};
    };
}