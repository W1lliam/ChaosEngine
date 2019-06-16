#include "ChaosEditor/ComponentsGUI/ChildComponentGUI.h"
#include <QFrame>
#include <QGridLayout>

namespace ChaosEditor
{
    ChildComponentGUI::ChildComponentGUI(QWidget* p_parent) : AComponentGUI("Child", p_parent)
    {
    }

    void ChildComponentGUI::SetupFrame()
    {
        CHAOS_ASSERT(m_component, "Drawing non bound ComponentGUI");
        QGridLayout* contentLayout = new QGridLayout;

        contentLayout->setSpacing(0);

        contentLayout->addWidget(new QLabel("Parent"),  0, 0, Qt::AlignLeft);
        contentLayout->addWidget(&m_parentName,         0, 1, Qt::AlignRight);

        contentLayout->setColumnStretch(0, 1);
        contentLayout->setColumnStretch(1, 2);
        setLayout(contentLayout);
    }

    void ChildComponentGUI::BindComponent(Chaos::IComponent* p_component)
    {
        Chaos::ChildComponent* childComponent = dynamic_cast<Chaos::ChildComponent*>(p_component);
        CHAOS_ASSERT(childComponent, "Wrong component type!");
        m_component = childComponent;
        m_parentName.setText((const char *)m_component->m_parent);
    }
}