#include "ChaosEditor\ComponentsGUI\AnimationComponentGUI.h"
#include <QComboBox>
#include <QCheckBox>
#include <QGridLayout>

namespace ChaosEditor
{
	AnimationComponentGUI::AnimationComponentGUI(QWidget* p_parent) :
		AComponentGUI("Animation", p_parent),
		m_selectedAnimation(new QComboBox),

		m_started(new QCheckBox),
		m_paused(new QCheckBox),
		m_stopped(new QCheckBox)
	{
		m_selectedAnimation->addItem("No Animation", (uintptr_t)(nullptr));
		const auto& meshes = Chaos::AnimationManager::Get().GetAllResources();
		for (const auto& pair : meshes)
		{
			m_selectedAnimation->addItem(pair.second->GetName().string, (uintptr_t)(pair.second.get()));
		}

		QObject::connect(m_started, &QCheckBox::stateChanged,
			this, &AnimationComponentGUI::StartAnimation);
		QObject::connect(m_paused, &QCheckBox::stateChanged,
			this, &AnimationComponentGUI::PauseAnimation);
		QObject::connect(m_paused, &QCheckBox::stateChanged,
			this, &AnimationComponentGUI::StopAnimation);
		QObject::connect(m_selectedAnimation, SIGNAL(currentIndexChanged(int)),
			this, SLOT(AnimationSelected(int)));
	}

	void ChaosEditor::AnimationComponentGUI::SetupFrame()
	{
		CHAOS_ASSERT(m_component, "Drawing AnimationComponentGUI");
		QGridLayout* contentLayout = new QGridLayout;

		contentLayout->setSpacing(0);
		contentLayout->addWidget(new QLabel("Selected Animation"), 0, 0, Qt::AlignLeft);
		contentLayout->addWidget(m_selectedAnimation, 0, 1, Qt::AlignRight);

		contentLayout->addWidget(new QLabel("ANIMATION BUTTONS"), 2, 0, Qt::AlignCenter);
		contentLayout->addWidget(new QLabel("START"), 3, 0, Qt::AlignLeft);
		contentLayout->addWidget(m_started, 3, 1, Qt::AlignRight);
		contentLayout->addWidget(new QLabel("PAUSE"), 4, 0, Qt::AlignLeft);
		contentLayout->addWidget(m_paused, 4, 1, Qt::AlignRight);
		contentLayout->addWidget(new QLabel("STOP"), 5, 0, Qt::AlignLeft);
		contentLayout->addWidget(m_stopped, 5, 1, Qt::AlignRight);

		contentLayout->setColumnStretch(0, 1);
		contentLayout->setColumnStretch(1, 2);
		setLayout(contentLayout);
	}

	void ChaosEditor::AnimationComponentGUI::BindComponent(Chaos::IComponent * p_component)
	{
		Chaos::AnimationComponent* animComponent = dynamic_cast<Chaos::AnimationComponent*>(p_component);
		CHAOS_ASSERT(animComponent, "Wrong component type!");
		m_component = animComponent;
		if (!m_component->GetSkeletalMesh())
		{
			m_selectedAnimation->setCurrentIndex(0);
			
			return;
		}
		m_selectedAnimation->setCurrentText(m_component->GetSkeletalMesh()->GetName().string);
		m_component->GetAnimator()->SetSkinningMode(Chaos::Animation::SkinningMode::DUALQUAT);
	}

	void AnimationComponentGUI::AnimationSelected(int p_index)
	{
		using namespace Chaos;
		Resources::SkeletalMesh* skelMesh = reinterpret_cast<Resources::SkeletalMesh*>(static_cast<uintptr_t>(m_selectedAnimation->currentData().toLongLong()));
		if (!skelMesh)
		{
			m_component->SetSkeletalMesh(nullptr);
			return;
		}
		if (m_component->GetSkeletalMesh())
		{
			if (skelMesh->GetName() == m_component->GetSkeletalMesh()->GetName())
				return;
		}
		if (skelMesh->GetAnimationMesh().size() > 0)
		{
			
			Resources::SkeletalMeshPtr toAssign = AnimationManager::Get().GetSkeletalMesh(skelMesh->GetName(), "Editor"_sid);
			m_component->SetSkeletalMesh(toAssign);
			uint32_t id = 0;
			std::unordered_map<uint32_t, EmptyGameObject&> createdSubMeshes(skelMesh->GetAnimationMesh().size());
			
			for (Resources::AnimMeshPtr submesh : skelMesh->GetAnimationMesh())
			{
				auto sceneSystem = SystemManager::Get().GetSystem<SceneSystem>();
				std::string name = submesh->GetAnimMeshName() + std::to_string(incrementor);
				std::cout << incrementor << std::endl;
				EmptyGameObject& empty = sceneSystem->CreateEntity<EmptyGameObject>(operator""_sid(name.c_str(), name.size()));
				empty.AddComponent<MaterialComponent>(MaterialManager::Get().GetMaterial("RevolverMaterial"_sid, "Editor"_sid));
				EntityID entityid = empty.GetEntityID();
				skelMesh->GetAnimationMesh()[id]->SetOwnerID(entityid);
				createdSubMeshes.emplace(id, empty);
				if (id != 0)
				{
					sceneSystem->DelayedSetParent(empty.GetEntityID(), createdSubMeshes.at(id - 1).GetEntityID(), sceneSystem->GetCurrentScene());
				}
				else
				{
					sceneSystem->DelayedSetParent(empty.GetEntityID(), m_component->GetOwnerID(), sceneSystem->GetCurrentScene());
				}
				++id;
				++incrementor;
			}
			m_selectedAnimation->setCurrentIndex(p_index);
		}
	}
	void ChaosEditor::AnimationComponentGUI::StartAnimation(bool p_triggerEnabled)
	{
		if (p_triggerEnabled)
		{	
				m_component->PlayAnimation();
				m_started->setChecked(p_triggerEnabled);
				m_stopped->setChecked(false);
				m_paused->setChecked(false);
		}
	}

	void ChaosEditor::AnimationComponentGUI::PauseAnimation(bool p_triggerEnabled)
	{
		if (p_triggerEnabled)
		{
			m_component->PauseAnimation();
			m_started->setChecked(false);
			m_stopped->setChecked(false);
			m_paused->setChecked(true);
		}
	}

	void ChaosEditor::AnimationComponentGUI::StopAnimation(bool p_triggerEnabled)
	{
		if (p_triggerEnabled)
		{
			m_component->StopAnimation();
			m_started->setChecked(false);
			m_paused->setChecked(false);
			m_stopped->setChecked(true);
		}
	}
}