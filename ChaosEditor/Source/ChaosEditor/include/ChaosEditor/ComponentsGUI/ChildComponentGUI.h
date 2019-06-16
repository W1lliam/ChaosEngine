#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"
#include <QLabel>

namespace ChaosEditor
{
    class ChildComponentGUI : public AComponentGUI
    {
        Q_OBJECT
    public:
        ChildComponentGUI(QWidget* p_parent = nullptr);
        ~ChildComponentGUI() = default;

        virtual void SetupFrame() override;
        virtual void BindComponent(Chaos::IComponent* p_component) override;

    protected:
        Chaos::ChildComponent*  m_component {nullptr};
        QLabel                  m_parentName;
    };
}