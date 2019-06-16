#include "ChaosEditor/ComponentsGUI/MeshComponentGUI.h"
#include <QFrame>
#include <QGridLayout>
#include <QComboBox>
#include <QVariant>

namespace ChaosEditor
{
    MeshComponentGUI::MeshComponentGUI(QWidget* p_parent) : 
        AComponentGUI("Mesh", p_parent),
        m_selectedMesh(new QComboBox),
        m_vertexCount("")
    {
        m_selectedMesh->addItem("No Mesh", (uintptr_t)(nullptr));
        const auto& meshes = Chaos::MeshManager::Get().GetAllResources();
        for (const auto& pair : meshes)
        {
            m_selectedMesh->addItem(pair.second->GetName().string, (uintptr_t)(pair.second.get()));
        }

        QObject::connect(m_selectedMesh,    SIGNAL(currentIndexChanged(int)),
                         this,              SLOT(MeshSelected(int)));
    }

    void MeshComponentGUI::SetupFrame()
    {
        CHAOS_ASSERT(m_component, "Drawing non bound ComponentGUI");
        QGridLayout* contentLayout = new QGridLayout;

        contentLayout->setSpacing(0);

        contentLayout->addWidget(new QLabel("Mesh"),            0, 0, Qt::AlignLeft);
        contentLayout->addWidget(m_selectedMesh,                0, 1, Qt::AlignRight);

        contentLayout->addWidget(new QLabel("Vertex Count"),    1, 0, Qt::AlignLeft);
        contentLayout->addWidget(&m_vertexCount,                1, 1, Qt::AlignRight);

        contentLayout->setColumnStretch(0, 1);
        contentLayout->setColumnStretch(1, 2);
        setLayout(contentLayout);
    }

    void MeshComponentGUI::BindComponent(Chaos::IComponent* p_component)
    {
        Chaos::MeshComponent* meshComponent = dynamic_cast<Chaos::MeshComponent*>(p_component);
        CHAOS_ASSERT(meshComponent, "Wrong component type!");
        m_component = meshComponent;
		if (!m_component->GetMesh())
		{
            m_selectedMesh->setCurrentIndex(0);
			return;
		}
        m_selectedMesh->setCurrentText(m_component->GetMesh()->GetOwner()->GetName().string);
        m_vertexCount.setText(QString::number(m_component->GetMesh()->GetVertexCount()));
    }

    void MeshComponentGUI::MeshSelected(int p_index)
    {
        using namespace Chaos;
        Resources::Mesh* mesh = reinterpret_cast<Resources::Mesh*>(static_cast<uintptr_t>(m_selectedMesh->currentData().toLongLong()));
        if (!mesh)
        {
            m_component->SetMesh(nullptr);
            m_vertexCount.setText("0");
        }
        else if (m_component->GetMesh() && mesh == m_component->GetMesh()->GetOwner())
        {
            return;
        }

        else if (mesh->GetSubMeshes().size() > 1)
        {
            MaterialComponent* material  = ComponentManager::Get().GetComponent<MaterialComponent>(m_component->GetOwnerID());
            std::unordered_map<std::string, EmptyGameObject&> createdSubMeshes(mesh->GetSubMeshes().size());
            for (Resources::SubMeshPtr submesh : mesh->GetSubMeshes())
            {
                auto sceneSystem = SystemManager::Get().GetSystem<SceneSystem>();
                EmptyGameObject& empty = sceneSystem->CreateEntity<EmptyGameObject>(operator""_sid(submesh->GetName().c_str(), submesh->GetName().size()));
                empty.GetComponent<TransformComponent>()->AsTransform().SetTransform(submesh->GetTransformation());
                empty.AddComponent<MeshComponent>(submesh);
                if (material && material->GetMaterial())
                {
                    empty.AddComponent<MaterialComponent>(material->GetMaterial());
                }
                else
                {
                    empty.AddComponent<MaterialComponent>(MaterialManager::Get().GetMaterial("Default"_sid));
                }
                createdSubMeshes.emplace(submesh->GetName(), empty);

                if (!submesh->GetParent().empty() && submesh->GetParent() != "RootNode")
                {
                    sceneSystem->DelayedSetParent(empty.GetEntityID(), createdSubMeshes.at(submesh->GetParent()).GetEntityID(), sceneSystem->GetCurrentScene());
                }
                else
                {
                    sceneSystem->DelayedSetParent(empty.GetEntityID(), m_component->GetOwnerID(), sceneSystem->GetCurrentScene());
                }
            }
        }
        else
        {
            MaterialComponent* material = ComponentManager::Get().GetComponent<MaterialComponent>(m_component->GetOwnerID());
            if (!material)
            {
                ComponentManager::Get().AddComponent<MaterialComponent>(m_component->GetOwnerID(), MaterialManager::Get().GetMaterial("Default"_sid));
            }
            m_component->SetMesh(mesh->GetSubMeshes()[0]);
            m_vertexCount.setText(QString::number(m_component->GetMesh()->GetVertexCount()));
        }
    }
}