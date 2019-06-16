#include "ChaosEditor/Inspector/Inspector.h"
#include <QTreeWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <ChaosEngine.h>
#include "ChaosEditor/Inspector/Category.h"
#include "ChaosEditor/ComponentsGUI/TransformComponentGUI.h"
#include "ChaosEditor/ComponentsGUI/ComponentGUIFactory.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QKeyEvent>
#include <QComboBox>
#include "ChaosEditor/Inspector/ComponentDialog.h"
#include "ChaosEditor/Inspector/RangeEdit.h"
#include "ChaosEditor/Inspector/MaterialGUI.h"
#include "ChaosEditor/CameraSystem.h"

namespace ChaosEditor
{
    Inspector::Inspector(QTreeWidget* p_container, QWidget* p_parent, Qt::WindowFlags p_flags) : 
        QDockWidget("INSPECTOR", p_parent, p_flags), 
        m_container(p_container),
        m_titleText(new QLineEdit)
    {
        m_titleText->installEventFilter(this);

		m_addComponentButton = new QPushButton("Add Component");
		m_addComponentButton->setVisible(false);
		QObject::connect(m_addComponentButton, SIGNAL(pressed()), this, SLOT(ShowComponentList()));

        m_container->setRootIsDecorated(false);
        m_container->setAlternatingRowColors(false);
        m_container->setIndentation(0);

        m_titleText->setStyleSheet("font-family: \"Open Sans Light\"; font-size: 18px; padding: 8px; background-color: #252526;");
        m_titleText->setHidden(true);
        m_titleText->setAlignment(Qt::AlignCenter);

        auto entityNameItem = new QTreeWidgetItem;
        m_container->addTopLevelItem(entityNameItem);
        m_container->setItemWidget(entityNameItem, 0, m_titleText);

		setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
		setFocusPolicy(Qt::FocusPolicy::StrongFocus);

		QVBoxLayout* boxLayout = new QVBoxLayout();
		boxLayout->addWidget(m_container);
		boxLayout->addWidget(m_addComponentButton);
		QFrame* frame = new QFrame();
		frame->setLayout(boxLayout);

		setWidget(frame);
		setAttribute(Qt::WA_WState_ExplicitShowHide);
    }

    Inspector::~Inspector()
    {
        m_titleText->deleteLater();
        for (auto& componentGUI : m_componentGUIs)
        {
            if (componentGUI.second)
            {
                delete componentGUI.second;
                componentGUI.second = nullptr;
            }
        }
		delete m_addComponentDialog;
    }

	Chaos::EntityPtr Inspector::GetEntity()
	{
		if(m_entity)
			return m_entity;
		return nullptr;
	}


    void Inspector::EntitySelected(QModelIndex p_index)
    {
        RemoveSceneGUI();
        RemoveMaterialGUI();

        m_titleText->setStyleSheet("font-family: \"Open Sans Light\"; font-size: 18px; padding: 8px; background-color: #1e1e1e;");
        if (p_index.parent().isValid())
        {
            m_entity = static_cast<Chaos::Scene::SceneNode*>(p_index.internalPointer())->entity;
            std::unordered_map<Chaos::ComponentTypeID, Chaos::ComponentID>&     components = m_entity->GetAllComponents();

            m_titleText->setVisible(true);
            m_addComponentButton->setVisible(true);

            m_titleText->setText(m_entity->GetEntityID().string);

            AddComponentGUIs(components);
            RemoveUnusedComponentGUIs(components);

        }
        else
        {
            m_entity = nullptr;
            Chaos::ScenePtr currentScene = Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene();
            m_titleText->setText(currentScene->GetName().string);
            std::unordered_map<Chaos::ComponentTypeID, Chaos::ComponentID> components;
            RemoveUnusedComponentGUIs(components);
            m_addComponentButton->setVisible(false);

            AddSceneGUI(currentScene);
        }
    }

    void Inspector::MaterialSelected(Chaos::Resources::MaterialPtr p_material)
    {
        RemoveSceneGUI();
        RemoveMaterialGUI();

        m_entity = nullptr;
        
        m_titleText->setStyleSheet("font-family: \"Open Sans Light\"; font-size: 18px; padding: 8px; background-color: #1e1e1e;");
        m_titleText->setVisible(true);
        m_titleText->setText(p_material->GetName().string);
        
        RemoveUnusedComponentGUIs({});
        m_addComponentButton->setVisible(false);

        m_materialGUI                = new QTreeWidgetItem;
        MaterialGUI* materialFrame   = new MaterialGUI(p_material);
        m_container->addTopLevelItem(m_materialGUI);
        m_container->setItemWidget  (m_materialGUI, 0, materialFrame);
    }

    void Inspector::ShowComponentList()
	{
		if (!m_addComponentDialog)
		{
			m_addComponentDialog = new ComponentDialog(this);
			m_addComponentDialog->setStyleSheet("QDialog { background-color: #383838;};");
			m_addComponentDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
			m_addComponentDialog->setSizeGripEnabled(false);

			connect(m_addComponentDialog->m_buttons["Transform"],			SIGNAL(pressed()), this,	SLOT(AddTransformComponent()));
			connect(m_addComponentDialog->m_buttons["Camera"],				SIGNAL(pressed()), this,	SLOT(AddCameraComponent()));
			connect(m_addComponentDialog->m_buttons["Box Collider"],		SIGNAL(pressed()), this,	SLOT(AddBoxComponent()));
			connect(m_addComponentDialog->m_buttons["Capsule Collider"],	SIGNAL(pressed()), this,	SLOT(AddCapsuleComponent()));
			connect(m_addComponentDialog->m_buttons["Sphere Collider"],		SIGNAL(pressed()), this,	SLOT(AddSphereComponent()));
			connect(m_addComponentDialog->m_buttons["Light"],				SIGNAL(pressed()), this,	SLOT(AddLightComponent()));
			connect(m_addComponentDialog->m_buttons["Material"],			SIGNAL(pressed()), this,	SLOT(AddMaterialComponent()));
			connect(m_addComponentDialog->m_buttons["Mesh"],				SIGNAL(pressed()), this,	SLOT(AddMeshComponent()));
			connect(m_addComponentDialog->m_buttons["RigidBody"],			SIGNAL(pressed()), this,	SLOT(AddRigidBodyComponent()));
			connect(m_addComponentDialog->m_buttons["Animation"],			SIGNAL(pressed()), this,	SLOT(AddAnimationComponent()));
			connect(m_addComponentDialog,	                                SIGNAL(AddedScriptedComponent(std::string)),    this,	SLOT(AddScriptedComponent(std::string)));
		}
		
		m_addComponentDialog->setGeometry(this->geometry().x(), this->geometry().y() + this->geometry().height() - m_addComponentButton->frameGeometry().height() - 200, this->geometry().width(), 200);
		m_addComponentDialog->ResizeScroll();

		m_addComponentDialog->show();
		m_addComponentDialog->raise();
		m_addComponentDialog->activateWindow();
		m_addComponentDialog->setFocus();
	}

    void Inspector::SystemStateChanged(bool p_stateEnabled)
    {
        Chaos::System* system = reinterpret_cast<Chaos::System*>(static_cast<uintptr_t>(sender()->property("System").toLongLong()));
        if (p_stateEnabled)
        {
            Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->AddSystem(system);
        }
        else
        {
            Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->RemoveSystem(system->GetSystemTypeID());
        }

		Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->EnableSystems();
    }

    void Inspector::SystemModeChanged(int p_systemMode)
    {
        Chaos::System* system = reinterpret_cast<Chaos::System*>(static_cast<uintptr_t>(sender()->property("System").toLongLong()));
        system->SetExecutionMode((Chaos::E_SystemExecutionModeFlags)(p_systemMode + 1));
    }
    void Inspector::SkyboxSelected(int p_skybox)
    {
        using namespace Chaos;
        Resources::ASkybox* cubemap = reinterpret_cast<Resources::ASkybox*>(static_cast<uintptr_t>(static_cast<QComboBox*>(sender())->currentData().toLongLong()));
        SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene()->SetSkybox(Resources::SkyboxPtr(cubemap));
    }

    void Inspector::ExposureChanged(double p_exposure)
    {
        Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->SetExposure((float)p_exposure);
    }

	void Inspector::BloomThresholdChanged(double p_bloom)
	{
		Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->SetBloomThreshold((float)p_bloom);
	}

	void Inspector::BloomBlurChanged(double p_blur)
	{
		Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->SetBloomBlurPasses((int)p_blur);
	}

	void Inspector::GammaChanged(double p_gamma)
	{
		Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->SetGamma((float)p_gamma);
	}
	void Inspector::BloomIntensityChanged(double p_bloomIntensity)
	{
		Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->SetBloomIntensity((float)p_bloomIntensity);
	}

    void Inspector::AddTransformComponent()
    {
        AddComponentGUI(m_entity->AddComponent<Chaos::TransformComponent>());
    }

    void Inspector::AddCameraComponent()
	{
        auto& camera = m_entity->AddComponent<Chaos::CameraComponent>();
        Chaos::SystemManager::Get().GetSystem<Chaos::CameraSystem>()->SetMainCamera(m_entity->GetEntityID());
		AddComponentGUI(camera);
	}

	void Inspector::AddBoxComponent()
	{
		AddComponentGUI(m_entity->AddComponent<Chaos::BoxColliderComponent>(Chaos::Maths::Vector3(0.6f, 0.7f, 0.0f), Chaos::Maths::Vector3(), Chaos::Maths::Vector3(1.0f, 1.0f, 1.0f)));
	}

	void Inspector::AddCapsuleComponent()
	{
		CHAOS_CORE_INFO("Capsule collider Added");
	}

	void Inspector::AddSphereComponent()
	{
		AddComponentGUI(m_entity->AddComponent<Chaos::SphereColliderComponent>(Chaos::Maths::Vector3(0.5f, 0.6f, 0.7f), Chaos::Maths::Vector3(), 1.5f));

	}

	void Inspector::AddLightComponent()
	{
		AddComponentGUI(m_entity->AddComponent<Chaos::LightComponent>());
	}

	void Inspector::AddMaterialComponent()
	{
		AddComponentGUI(m_entity->AddComponent<Chaos::MaterialComponent>());
	}

	void Inspector::AddMeshComponent()
	{
		AddComponentGUI(m_entity->AddComponent<Chaos::MeshComponent>());
	}

	void Inspector::AddRigidBodyComponent()
	{
		AddComponentGUI(m_entity->AddComponent<Chaos::RigidBodyComponent>(Chaos::E_RigidBodyType::DYNAMIC));
	}

	void Inspector::AddAnimationComponent()
	{
		AddComponentGUI(m_entity->AddComponent<Chaos::AnimationComponent>());
	}

    void Inspector::AddScriptedComponent(std::string p_componentType)
    {
        if (!filesystem::exists("Assets/Scripts/Components/" + p_componentType + ".nut"))
        {
            std::ofstream out("Assets/Scripts/Components/" + p_componentType + ".nut", std::ios::out);
            if (out.is_open())
            {
                out << "class " + p_componentType + " extends SqComponent\n\
{\n\
    constructor(p_cppComponent)\n\
    {\n\
        base.constructor(p_cppComponent);\n\
    }\n\
}";
            }
            out.close();
            Chaos::ScriptsHandler::Get().CompileComponent("Assets/Scripts/Components/" + p_componentType + ".nut");
        }
        AddComponentGUI(m_entity->AddScriptedComponent(p_componentType), p_componentType);
    }

	void Inspector::OnComponentRemoved()
	{
		if(m_entity)
			RemoveUnusedComponentGUIs(m_entity->GetAllComponents());
	}

    bool Inspector::eventFilter(QObject * obj, QEvent * event)
    {
        if (obj == m_titleText)
        {
            if (event->type() == QEvent::FocusAboutToChange)
            {
                TitleChanged(m_titleText->text().toStdString());
            }
            else if (event->type() == QEvent::KeyPress)
            {
                QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
                if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
                {
                    TitleChanged(m_titleText->text().toStdString());
                    m_titleText->clearFocus();
                }
            }
        }
        return QObject::eventFilter(obj, event);
    }

    void Inspector::TitleChanged(std::string p_newTitle)
    {
        if (m_entity)
        {
            m_entity->Rename(p_newTitle);
        }
        else if (m_systemsGUI)
        {
            Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->Rename(p_newTitle);
        }
        else if (m_materialGUI)
        {
            static_cast<MaterialGUI*>(m_container->itemWidget(m_materialGUI, 0))->GetMaterial()->Rename(p_newTitle);
        }
    }

	void Inspector::AddComponentGUI(Chaos::IComponent& p_component, std::string p_scriptedComponentType)
	{
		if (m_componentGUIs.find(p_component.GetComponentTypeID()) != m_componentGUIs.end())
		{
			m_componentGUIs[p_component.GetComponentTypeID()]->BindComponent(&p_component);
		}
		else
		{
            AComponentGUI* componentGUI = nullptr;
            if(p_scriptedComponentType.empty())
                componentGUI = ComponentGUIFactory::CreateComponentGUI(p_component.GetComponentTypeID());
            else
                componentGUI = ComponentGUIFactory::CreateScriptedComponentGUI(p_scriptedComponentType);

			if (!componentGUI)
			{
				CHAOS_WARN("Component type has no GUI");
				return;
			}
			m_componentGUIs[p_component.GetComponentTypeID()] = componentGUI;

			componentGUI->BindComponent(&p_component);
			componentGUI->SetupFrame();
			componentGUI->AddToTree(m_container, &p_component);

			Category* category = componentGUI->GetCategory();
			if (category)
				connect(category, SIGNAL(ComponentRemoved()), this, SLOT(OnComponentRemoved()));
		}
	}

    void Inspector::AddSceneGUI(Chaos::ScenePtr & p_scene)
    {
		using namespace Chaos;

                            m_systemsGUI    = new QTreeWidgetItem;
        QFrame*             systemFrame     = new QFrame;
        m_container->addTopLevelItem(m_systemsGUI);
        m_container->setItemWidget(m_systemsGUI, 0, systemFrame);
        
        QGridLayout* layout = new QGridLayout;

        QComboBox* skyBoxComboBox = new QComboBox(systemFrame);

        const auto& skyboxes = Chaos::SkyboxManager::Get().GetAllResources();
        for (const auto& pair : skyboxes)
        {
            skyBoxComboBox->addItem(pair.second->GetName().string, (uintptr_t)(pair.second.get()));
        }
        skyBoxComboBox->setCurrentText(p_scene->GetSkybox()->GetName().string);

        layout->addWidget(new QLabel("Skybox"), 0, 0, Qt::AlignLeft);
        layout->addWidget(skyBoxComboBox,       0, 1, Qt::AlignRight);
        layout->setRowMinimumHeight(0, 10);

        const SceneRenderParam& sceneRenderParams = p_scene->GetRenderParams();

        RangeEdit* exposureRange = new RangeEdit;
        exposureRange->SetMultiplier(1000.0f);
        exposureRange->SetMinValue(0.1f);
        exposureRange->SetMaxValue(15.0f);
        exposureRange->SetValue(sceneRenderParams.skyboxExposure);

        layout->addWidget(new QLabel("Exposure"),   1, 0, Qt::AlignLeft);
        layout->addWidget(exposureRange,                1, 1, Qt::AlignRight);
        layout->setRowMinimumHeight(1, 30);

		RangeEdit* bloomIntensitySlider = new RangeEdit;

		bloomIntensitySlider->SetMultiplier(10000.0f);
		bloomIntensitySlider->SetMinValue(0.0f);
		bloomIntensitySlider->SetMaxValue(2.0f);
		bloomIntensitySlider->SetValue(sceneRenderParams.bloomIntesity);

		layout->addWidget(new QLabel("Bloom Intensity"), 2, 0, Qt::AlignLeft);
		layout->addWidget(bloomIntensitySlider, 2, 1, Qt::AlignRight);
		layout->setRowMinimumHeight(2, 30);


		RangeEdit* bloomSlider = new RangeEdit;

		bloomSlider->SetMultiplier(1000.0f);
		bloomSlider->SetMinValue(0.5f);
		bloomSlider->SetMaxValue(50.0f);
		bloomSlider->SetValue(sceneRenderParams.bloomThreshold);

		layout->addWidget(new QLabel("Bloom Threshold"), 3, 0, Qt::AlignLeft);
		layout->addWidget(bloomSlider, 3, 1, Qt::AlignRight);
		layout->setRowMinimumHeight(3, 30);

		RangeEdit* bloomBlurSlider = new RangeEdit;

		bloomBlurSlider->SetMinValue(0.0f);
		bloomBlurSlider->SetMaxValue(10.0f);
		bloomBlurSlider->SetValue(sceneRenderParams.bloomBlurPasses);

		layout->addWidget(new QLabel("Bloom Blur Passes"), 4, 0, Qt::AlignLeft);
		layout->addWidget(bloomBlurSlider, 4, 1, Qt::AlignRight);
		layout->setRowMinimumHeight(4, 30);

		RangeEdit* gammaSlider = new RangeEdit;

		gammaSlider->SetMultiplier(1000.0f);
		gammaSlider->SetMinValue(1.0f);
		gammaSlider->SetMaxValue(3.0f);
		gammaSlider->SetValue(sceneRenderParams.gamma);

		layout->addWidget(new QLabel("Gamma"), 5, 0, Qt::AlignLeft);
		layout->addWidget(gammaSlider, 5, 1, Qt::AlignRight);
		layout->setRowMinimumHeight(5, 30);


        QObject::connect(skyBoxComboBox,		SIGNAL(currentIndexChanged(int)),
                         this,					SLOT(SkyboxSelected(int)));
        
        QObject::connect(exposureRange,			SIGNAL(ValueChanged(double)),
                         this,					SLOT(ExposureChanged(double)));

		QObject::connect(bloomSlider,			SIGNAL(ValueChanged(double)),
						 this,					SLOT(BloomThresholdChanged(double)));

		QObject::connect(bloomBlurSlider,		SIGNAL(ValueChanged(double)),
						 this,					SLOT(BloomBlurChanged(double)));

		QObject::connect(bloomIntensitySlider,	SIGNAL(ValueChanged(double)),
						 this,					SLOT(BloomIntensityChanged(double)));

		QObject::connect(gammaSlider,			SIGNAL(ValueChanged(double)),
						 this,					SLOT(GammaChanged(double)));


        layout->setColumnStretch(0, 0);
        layout->setColumnStretch(1, 5);
        layout->setColumnStretch(2, 5);
        layout->setColumnStretch(3, 5);
        layout->setColumnStretch(4, 5);
        layout->setColumnStretch(5, 5);
        
        int row = 6;
        Chaos::SystemManager::Get().foreach([&] (Chaos::System* p_system)
        {
            if (p_system->GetSystemTypeID() == ChaosEditor::CameraSystem::SYSTEM_TYPE_ID)
                return;
            QCheckBox* enableCheckBox = new QCheckBox(systemFrame);
            enableCheckBox->setChecked(p_scene->GetSystem(p_system->GetSystemTypeID()) != nullptr);
            enableCheckBox->setProperty("System", (uintptr_t)(p_system));
        
            QComboBox* modeComboBox = new QComboBox(systemFrame);
            modeComboBox->setProperty( "System",       (uintptr_t)(p_system));
            modeComboBox->addItem(     "Editor",       (uint8_t)Chaos::E_SystemExecutionModeFlags::EDITOR);
            modeComboBox->addItem(     "Game",         (uint8_t)Chaos::E_SystemExecutionModeFlags::GAME);
            modeComboBox->addItem(     "Both",         (uint8_t)Chaos::E_SystemExecutionModeFlags::BOTH);
            int index = (p_system->GetExecutionMode() & Chaos::E_SystemExecutionModeFlags::EDITOR) ? 
                            ((p_system->GetExecutionMode() & Chaos::E_SystemExecutionModeFlags::GAME) ? 2 : 0) : 1;
                             modeComboBox->setCurrentIndex(index);

            std::string name = p_system->GetSystemTypeName();
            auto column = name.find_last_of(':');
            if(column != std::string::npos)
                name = name.substr(column + 1);
            layout->addWidget(enableCheckBox,                   row, 0, Qt::AlignLeft);
            layout->addWidget(new QLabel(name.c_str(), systemFrame),    row, 1, Qt::AlignLeft);
            layout->addWidget(modeComboBox,                     row, 2, Qt::AlignRight);


            layout->setRowMinimumHeight(row, 25);

            QObject::connect(   enableCheckBox, &QCheckBox::stateChanged,
                                this,           &Inspector::SystemStateChanged);
            QObject::connect(   modeComboBox,   SIGNAL(currentIndexChanged(int)),
                                this,           SLOT(SystemModeChanged(int)));
            row++;
        });
        systemFrame->setMinimumHeight(row * 35);
        systemFrame->setLayout(layout);
    }

    void Inspector::RemoveSceneGUI()
    {
        if (m_systemsGUI)
        {
            delete m_systemsGUI;
            m_systemsGUI = nullptr;
        }
    }

    void Inspector::RemoveMaterialGUI()
    {
        if (m_materialGUI)
        {
            delete m_materialGUI;
            m_materialGUI = nullptr;
        }
    }

	void Inspector::AddComponentGUIs(const std::unordered_map<Chaos::ComponentTypeID, Chaos::ComponentID>& p_components)
    {
        for (const std::pair<Chaos::ComponentTypeID, Chaos::ComponentID>& componentIDs : p_components)
        {
            Chaos::IComponent* component = Chaos::ComponentManager::Get().GetComponent(componentIDs.second);
            if (m_componentGUIs.find(componentIDs.first) != m_componentGUIs.end())
            {
                m_componentGUIs[componentIDs.first]->BindComponent(component);
            }
            else
            {
                AComponentGUI* componentGUI = ComponentGUIFactory::CreateComponentGUI(componentIDs.first);
                if (!componentGUI)
                {
                    CHAOS_WARN("Component type has no GUI");
                    continue;
                }
                m_componentGUIs[componentIDs.first] = componentGUI;

                componentGUI->BindComponent(component);
                componentGUI->SetupFrame();
                componentGUI->AddToTree(m_container, component);

				Category* category = componentGUI->GetCategory();
				if(category)
					connect(category, SIGNAL(ComponentRemoved()), this, SLOT(OnComponentRemoved()));
            }
        }
    }

    void Inspector::RemoveUnusedComponentGUIs(const std::unordered_map<Chaos::ComponentTypeID, Chaos::ComponentID>& p_components)
    {
        for (auto it = std::begin(m_componentGUIs); it != std::end(m_componentGUIs);)
        {
            if (std::find_if(p_components.begin(), p_components.end(), [&it](const std::pair<Chaos::ComponentTypeID, Chaos::ComponentID>& p_comp) {return p_comp.first == it->first; }) == p_components.end())
            {
                it->second->RemoveFromTree(m_container);
                it->second->deleteLater();
                it = m_componentGUIs.erase(it);
            }
            else
                ++it;
        }
    }

}