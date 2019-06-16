#include "ChaosEditor/Inspector/DragLabel.h"
#include <QMouseEvent>

namespace ChaosEditor
{
    DragLabel::DragLabel(const QString& p_label, QWidget* p_parent) : QLabel(p_label, p_parent)
    {}

    void DragLabel::mousePressEvent(QMouseEvent* p_event)
    {
        emit DragStart(p_event);
    }
    void DragLabel::mouseMoveEvent(QMouseEvent * p_event)
    {
        emit Drag(p_event);
    }
    void DragLabel::mouseReleaseEvent(QMouseEvent * p_event)
    {
        emit DragStop(p_event);
    }

    void DragLabel::enterEvent(QEvent * ev)
    {
        setCursor(QCursor(Qt::SizeHorCursor));
    }
    void DragLabel::leaveEvent(QEvent * ev)
    {
        setCursor(QCursor(Qt::ArrowCursor));
    }
}