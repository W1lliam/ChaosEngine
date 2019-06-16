#include "ChaosEditor/ComponentsGUI/ScriptedComponentGUI.h"
#include "ChaosEditor/Inspector/Vector3Widget.h"
#include "ChaosEditor/Inspector/QuaternionWidget.h"
#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>
#include <QLabel>

namespace ChaosEditor
{
    ScriptedComponentGUI::ScriptedComponentGUI(std::string p_componentType, QWidget* p_parent) : AComponentGUI(p_componentType, p_parent)
    {
    }

    void ScriptedComponentGUI::SetupFrame()
    {
        QGridLayout* contentLayout = new QGridLayout;

        contentLayout->setSpacing(0);

        contentLayout->setColumnStretch(0, 1);
        contentLayout->setColumnStretch(1, 2);
        setLayout(contentLayout);
    }

    void ScriptedComponentGUI::BindComponent(Chaos::IComponent* p_component)
    {
        Chaos::ScriptedComponent* scriptedComponent = dynamic_cast<Chaos::ScriptedComponent*>(p_component);
        CHAOS_ASSERT(scriptedComponent, "Wrong component type!");

        m_component = scriptedComponent;

    }
}