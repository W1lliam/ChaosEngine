#include "ChaosEditor/ComponentsGUI/AudioSourceComponentGUI.h"
#include "ChaosEditor/Inspector/Vector3Widget.h"
#include "ChaosEditor/Inspector/QuaternionWidget.h"
#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>
#include <QLabel>

namespace ChaosEditor
{
    AudioSourceComponentGUI::AudioSourceComponentGUI(QWidget* p_parent) : AComponentGUI("AudioSource", p_parent)
    {
    }

    void AudioSourceComponentGUI::SetupFrame()
    {
        QGridLayout* contentLayout = new QGridLayout;

        contentLayout->setSpacing(0);


        contentLayout->setColumnStretch(0, 1);
        contentLayout->setColumnStretch(1, 2);
        setLayout(contentLayout);
    }

    void AudioSourceComponentGUI::BindComponent(Chaos::IComponent* p_component)
    {
        Chaos::AudioSourceComponent* audioSourceComponent = dynamic_cast<Chaos::AudioSourceComponent*>(p_component);
        CHAOS_ASSERT(audioSourceComponent, "Wrong component type!");

        m_component = audioSourceComponent;

    }
}