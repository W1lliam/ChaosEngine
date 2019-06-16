#pragma once
#include <QPushButton>
#include <QTreeWidget>
#include "ChaosCore.h"

namespace ChaosEditor
{

    class Category : public QPushButton
    {
        Q_OBJECT
    public:
        Category(QTreeWidget* p_parent, const QString& p_text,
            QTreeWidgetItem* p_item);

		void SetComponentType(Chaos::IComponent* p_type);

	signals:
		void ComponentRemoved();

    private slots:
        void ButtonPressed();
		void ShowContextMenu(const QPoint&);

    private:
        QTreeWidgetItem*		m_item;
		QString					m_name;
		Chaos::IComponent*		m_componentType{};

    };
}