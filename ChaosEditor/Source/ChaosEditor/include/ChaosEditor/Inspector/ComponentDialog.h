#pragma once

#include <map>
#include <QDialog>

class QVBoxLayout;
class QScrollArea;
class QPushButton;

namespace ChaosEditor
{
	class ComponentDialog: public QDialog
	{
		Q_OBJECT
	public slots:

	public:
		explicit ComponentDialog(QWidget* parent = nullptr);
		~ComponentDialog();

		void ResizeScroll();

	private:
		void AddButon(QString p_name);
		
    signals:
        void AddedScriptedComponent(std::string p_name);

	protected:
        bool eventFilter(QObject* obj, QEvent *event);
		void focusOutEvent(QFocusEvent* event)	override;
		void focusInEvent(QFocusEvent* event)	override;

		// Variables
	public:
		std::map<std::string, QWidget*>	m_buttons;

	private:
		QVBoxLayout*						m_layout;
		QScrollArea*						m_scroll;

	};
}