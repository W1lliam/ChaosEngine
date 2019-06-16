#pragma once
#include <QLabel>
#include <QMouseEvent>

namespace ChaosEditor
{
    class DragLabel : public QLabel
    {
        Q_OBJECT
    public:
        DragLabel(const QString& p_label, QWidget* p_parent = nullptr);

    signals:
        void DragStart(QMouseEvent* p_event);
        void Drag(QMouseEvent* p_event);
        void DragStop(QMouseEvent* p_event);

    protected:
        void mousePressEvent(QMouseEvent* p_event) override;
        void mouseMoveEvent(QMouseEvent* p_event) override;
        void mouseReleaseEvent(QMouseEvent* p_event) override;

        void enterEvent(QEvent *ev) override;
        void leaveEvent(QEvent *ev) override;
    };
}