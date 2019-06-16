#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"

namespace ChaosEditor
{
    class Vector3Widget;
    class QuaternionWidget;

    class ScriptedComponentGUI : public AComponentGUI
    {
        Q_OBJECT
    public:
        ScriptedComponentGUI(std::string p_componentType, QWidget* p_parent = nullptr);
        ~ScriptedComponentGUI() = default;

        virtual void SetupFrame() override;
        virtual void BindComponent(Chaos::IComponent* p_component) override;

    public slots:

    protected:
        Chaos::ScriptedComponent*  m_component     {nullptr};
    };
}