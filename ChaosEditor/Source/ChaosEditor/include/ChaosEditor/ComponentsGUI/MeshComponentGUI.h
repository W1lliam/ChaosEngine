#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"
#include <QLabel>

class QComboBox;

namespace ChaosEditor
{
    class MeshComponentGUI : public AComponentGUI
    {
        Q_OBJECT
    public:
        MeshComponentGUI(QWidget* p_parent = nullptr);
        ~MeshComponentGUI() = default;

        virtual void SetupFrame() override;
        virtual void BindComponent(Chaos::IComponent* p_component) override;

    public slots:
        void MeshSelected(int p_index);

    protected:
        Chaos::MeshComponent*   m_component     {nullptr};
        QComboBox*              m_selectedMesh  {nullptr};
        QLabel                  m_vertexCount;
    };
}