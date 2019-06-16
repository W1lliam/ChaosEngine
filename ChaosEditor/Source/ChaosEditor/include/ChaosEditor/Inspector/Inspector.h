#pragma once
#include <QDockWidget>
#include <unordered_map>
#include <ChaosCore.h>


class QTreeWidget;
class QLineEdit;
class QPushButton;
class QTreeWidgetItem;

namespace ChaosEditor
{
	class ComponentDialog;
    class AComponentGUI;

    class Inspector : public QDockWidget
    {
        Q_OBJECT
    public:
        explicit Inspector(QTreeWidget* p_container, QWidget* p_parent = nullptr, Qt::WindowFlags p_flags = Qt::WindowFlags());
        ~Inspector();

		Chaos::EntityPtr GetEntity();

    public slots:
        void EntitySelected(QModelIndex p_index);
        void MaterialSelected(Chaos::Resources::MaterialPtr p_material);
        void ShowComponentList();
        void SystemStateChanged(bool p_stateEnabled);
        void SystemModeChanged(int p_systemMode);
        void SkyboxSelected(int p_skybox);
        void ExposureChanged(double p_exposure);
        void BloomThresholdChanged(double p_bloom);
        void BloomBlurChanged(double p_blur);
        void GammaChanged(double p_gamma);
        void BloomIntensityChanged(double p_intensity);

        void AddTransformComponent();
		void AddCameraComponent();
		void AddBoxComponent();
		void AddCapsuleComponent();
		void AddSphereComponent();
		void AddLightComponent();
		void AddMaterialComponent();
		void AddMeshComponent();
		void AddRigidBodyComponent();
		void AddAnimationComponent();
		void AddScriptedComponent(std::string p_componentType);

		void OnComponentRemoved();
    protected:
        bool eventFilter(QObject* obj, QEvent *event);
        void TitleChanged(std::string p_newTitle);

    private:
        void AddComponentGUIs(const std::unordered_map<Chaos::ComponentTypeID, Chaos::ComponentID>& p_components);
        void AddComponentGUI(Chaos::IComponent& p_component, std::string p_scriptedComponentType = "");
        void AddSceneGUI(Chaos::ScenePtr& p_scene);
        void RemoveSceneGUI();
        void RemoveMaterialGUI();
        void RemoveUnusedComponentGUIs(const std::unordered_map<Chaos::ComponentTypeID, Chaos::ComponentID>& p_components);
    private:
		Chaos::EntityPtr	m_entity;
        QTreeWidget*		m_container;
        QLineEdit*			m_titleText;
		QPushButton*		m_addComponentButton;
		ComponentDialog*	m_addComponentDialog = nullptr;
        std::unordered_map<Chaos::ComponentTypeID, AComponentGUI*>  m_componentGUIs{};
        QTreeWidgetItem*                                            m_systemsGUI{nullptr};
        QTreeWidgetItem*                                            m_materialGUI{nullptr};
    };
}