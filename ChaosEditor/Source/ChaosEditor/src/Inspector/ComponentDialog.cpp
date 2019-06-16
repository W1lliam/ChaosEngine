#include "ChaosEditor/Inspector/ComponentDialog.h"

#include <ChaosEngine.h>
#include <QFocusEvent>
#include <QVBoxLayout>
#include <string>
#include <QScrollArea>
#include <QPushButton>
#include <QFrame>
#include <QLineEdit>

namespace ChaosEditor
{
	ComponentDialog::ComponentDialog(QWidget* parent) : QDialog(parent)
	{
		m_layout		= new QVBoxLayout();
		m_scroll		= new QScrollArea(this);
		ResizeScroll();
		m_scroll->setWidgetResizable(true);

		QFrame* frame	= new QFrame(this);
		frame->setLayout(m_layout);
		m_scroll->setWidget(frame);

		AddButon("Transform");
		AddButon("Camera");
		AddButon("Box Collider");
		AddButon("Capsule Collider");
		AddButon("Sphere Collider");
		AddButon("Light");
		AddButon("Material");
		AddButon("Mesh");
		AddButon("RigidBody");
		AddButon("Animation");


        QLineEdit* scriptedComp = new QLineEdit("ScriptedComponent");
        scriptedComp->installEventFilter(this);
        m_buttons["ScriptedComponent"] = scriptedComp;
        scriptedComp->setMinimumHeight(20);scriptedComp->setStyleSheet("QPushButton { background-color: #232323;};");
        m_layout->addWidget(scriptedComp);
	}

	ComponentDialog::~ComponentDialog()
	{

	}

	void ComponentDialog::ResizeScroll()
	{
		m_scroll->setGeometry(0,0,width(), height());
	}

	void ComponentDialog::AddButon(QString p_name)
	{
		m_buttons[p_name.toStdString()] = new QPushButton(p_name);
		m_buttons[p_name.toStdString()]->setMinimumHeight(20);
		m_buttons[p_name.toStdString()]->setStyleSheet("QPushButton { background-color: #232323;};");
		m_layout->addWidget(m_buttons[p_name.toStdString()]);
	}

    bool ComponentDialog::eventFilter(QObject * obj, QEvent * event)
    {
        if (obj == m_buttons["ScriptedComponent"])
        {
            if (event->type() == QEvent::KeyPress)
            {
                QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
                if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
                {
                    emit AddedScriptedComponent(static_cast<QLineEdit*>(m_buttons["ScriptedComponent"])->text().toStdString());
                    clearFocus();
                    hide();
                }
            }
        }
        return QObject::eventFilter(obj, event);
    }

    void ComponentDialog::focusOutEvent(QFocusEvent* event)
	{
		if (event->lostFocus() && !m_buttons["ScriptedComponent"]->hasFocus())
		{
			hide();
		}
	}

	void ComponentDialog::focusInEvent(QFocusEvent * event)
	{
	}


}