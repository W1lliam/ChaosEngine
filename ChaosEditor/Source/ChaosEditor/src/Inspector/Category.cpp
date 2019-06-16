#include "ChaosEditor/Inspector/Category.h"
#include "ChaosEditor/Inspector/Inspector.h"

#include <QMenu>

namespace ChaosEditor
{
    Category::Category(QTreeWidget* p_parent, const QString& p_text, QTreeWidgetItem* p_item)
        : QPushButton(p_text.toUpper(), p_parent), m_name(p_text.toUpper())
        , m_item(p_item)
    {
		setContextMenuPolicy(Qt::CustomContextMenu);
		connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
        connect(this, SIGNAL(pressed()), this, SLOT(ButtonPressed()));
        m_item->setExpanded(true);
    }

    void Category::ButtonPressed()
    {
        m_item->setExpanded(!m_item->isExpanded());
    }

	void Category::SetComponentType(Chaos::IComponent* p_type)
	{
		m_componentType = p_type;
	}

	void Category::ShowContextMenu(const QPoint& p_pos)
	{
		QPoint globalPos = mapToGlobal(p_pos);

		QMenu menu;

		menu.addAction("Remove Component");
		QAction* selectedItem = menu.exec(globalPos);

		if (selectedItem)
		{
			Chaos::ComponentManager::Get().RemoveComponent(m_componentType->GetOwnerID(), m_componentType->GetComponentTypeID());
			emit ComponentRemoved();
		}
	}
}