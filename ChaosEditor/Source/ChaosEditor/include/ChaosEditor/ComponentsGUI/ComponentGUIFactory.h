#pragma once
#include <ChaosCore.h>

class QWidget;

namespace ChaosEditor
{
    class AComponentGUI;
    class ComponentGUIFactory
    {
    public:
        ComponentGUIFactory() = delete;
        static AComponentGUI* CreateComponentGUI(Chaos::ComponentTypeID p_componentType, QWidget* p_parent = nullptr);
        static AComponentGUI* CreateScriptedComponentGUI(const std::string& p_scriptedComponentType, QWidget* p_parent = nullptr);
    };
}