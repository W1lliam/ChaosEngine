#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"

namespace ChaosEditor
{
    class Vector3Widget;
    class QuaternionWidget;

    class AudioSourceComponentGUI : public AComponentGUI
    {
        Q_OBJECT
    public:
        AudioSourceComponentGUI(QWidget* p_parent = nullptr);
        ~AudioSourceComponentGUI() = default;

        virtual void SetupFrame() override;
        virtual void BindComponent(Chaos::IComponent* p_component) override;

    public slots:

    protected:
        Chaos::AudioSourceComponent*  m_component     {nullptr};
    };
}