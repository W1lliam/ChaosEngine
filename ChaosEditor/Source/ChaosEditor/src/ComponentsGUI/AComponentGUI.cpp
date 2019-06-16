#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"
#include "ChaosEditor/Inspector/Category.h"
#include "ChaosCore.h"
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <ChaosDebug.h>


namespace ChaosEditor
{
    AComponentGUI::AComponentGUI(const std::string& p_name, QWidget* p_parent) :
        QFrame(p_parent),
        m_name(p_name),
        m_titleContainer(new QTreeWidgetItem()),
        m_contentContainer(new QTreeWidgetItem())
    {
        m_contentContainer->setDisabled(true);
        m_titleContainer->addChild(m_contentContainer);
    }

    void AComponentGUI::AddToTree(QTreeWidget* p_container, Chaos::IComponent* p_component)
    {
        CHAOS_ASSERT(p_container, "ComponentGUIs need to be setup in a QTreeWidget");

        p_container->addTopLevelItem(m_titleContainer);

        if (!m_category)
        {
            m_category = new Category(p_container, m_name.c_str(), m_titleContainer);

			m_category->SetComponentType(p_component);
        }
        else
        {
            m_category->setParent(p_container);
        }
        p_container->setItemWidget(m_titleContainer, 0, m_category);
        p_container->setItemWidget(m_contentContainer, 0, this);
    }

    void AComponentGUI::RemoveFromTree(QTreeWidget * p_container)
    {
        m_titleContainer->removeChild(m_contentContainer);
        delete m_titleContainer;
    }
}