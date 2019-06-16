#include "ChaosEditor/MenuBar.h"
#include "ChaosEditor/editor.h"
#include "ChaosEditor/Inspector/Inspector.h"
#include "ChaosAssetsManager.h"
#include "ui_editor.h"
#include <ChaosCore.h>
#include "ChaosEditor/CameraSystem.h"

#include <QAction>
#include <QFileDialog>

namespace ChaosEditor
{
    MenuBar::MenuBar(Ui::Editor* p_ui)
    {
        s_ui = p_ui;
        QAction* newScene = new QAction("New Scene");
        newScene->setShortcut(QKeySequence::New);
        s_ui->menuFiles->addAction(newScene);
        QObject::connect(newScene,          &QAction::triggered,    &MenuBar::CreateNewScene);

        QObject::connect(s_ui->actionSave,  &QAction::triggered,    &MenuBar::SaveScene);
        s_ui->actionSave->setShortcut(QKeySequence::Save);

        QAction* loadScene = new QAction("Load Scene");
        s_ui->menuFiles->addAction(loadScene);
        QObject::connect(loadScene,         &QAction::triggered,    &MenuBar::LoadScene);


       

        QMenu*		gameObject  = s_ui->menubar->addMenu("&GameObject");
        QAction*	createEmpty = gameObject->addAction("Create Empty");
        createEmpty->setStatusTip("Creates an empty gameobject");
        QObject::connect(createEmpty,       &QAction::triggered,    &MenuBar::CreateEmptyGameObject);

        QMenu*		resources       = s_ui->menubar->addMenu("&Resources");
        QAction*	importAsset     = resources->addAction("Import Asset");
        importAsset->setStatusTip("Import Asset file");


        QAction*	createMaterial  = resources->addAction("Create Material");
        createMaterial->setStatusTip("Creates a new material");

        QObject::connect(importAsset,       &QAction::triggered,    &MenuBar::ImportAsset);
        QObject::connect(createMaterial,    &QAction::triggered,    &MenuBar::CreateMaterial);
    }

    void MenuBar::CreateNewScene()
    {
        using namespace Chaos;
        int         copies  = 0;
        std::string name    = "New Scene" + std::to_string(copies);
        StringID	nameID  = operator""_sid(name.c_str(), name.size());

        while (SceneManager::Get().GetScene(nameID))
        {
            ++copies;
            name    = "New Scene" + std::to_string(copies);
            nameID  = operator""_sid(name.c_str(), name.size());
        }
        auto newScene = std::reinterpret_pointer_cast<Scene>(SceneManager::Get().CreateOrRetrieve(name, "Editor"_sid).first);
        SystemManager::Get().GetSystem<Chaos::SceneSystem>()->SetCurrentScene(newScene);
		SystemManager::Get().GetSystem<ChaosEditor::CameraSystem>()->Enable();
        
        std::string filepath  = "Assets/";
        if (!std::filesystem::is_directory(filepath))
            std::filesystem::create_directory(filepath);

        std::ofstream dataStream(filepath + newScene->GetName().string + ".resdecl", std::ios::out | std::ios::trunc);

        if (dataStream.is_open())
        {
            dataStream << "#Groups\n";
            dataStream << newScene->GetFullName() + "\n\n";
            dataStream << "#Locations\n";
            dataStream << "," + newScene->GetFullName() + ",true\n\n";
            dataStream << "#Resources";
        }
        dataStream.close();
    }

    void MenuBar::SaveScene()
    {
        using namespace Chaos;
        std::string check = "New Scene";
        if (SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->GetName() == operator""_sid(check.c_str(), check.size()))
        {
            QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Scene", QApplication::applicationDirPath() + "/Assets/Scenes/", "Scene Files (*.chaosscene)");
            std::filesystem::path path(fileName.toStdString());
            if (!path.stem().string().empty())
            {
                SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->Rename(path.stem().string());
                SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->Save();
            }
        }
        else
        {
            SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->Save();
        }
    }

    void MenuBar::LoadScene()
    {
        using namespace Chaos;
        QString fileName = QFileDialog::getOpenFileName(nullptr, "Load Scene", QApplication::applicationDirPath() + "/Assets/Scenes/", "Scene Files (*.chaosscene)");
        std::filesystem::path path(fileName.toStdString());

        CHAOS_CORE_TRACE(fileName.toStdString());

        if (!path.stem().string().empty())
        {
            
            auto scene       = std::reinterpret_pointer_cast<Scene>(SceneManager::Get().CreateOrRetrieve(path.stem().string()).first);
            SystemManager::Get().GetSystem<SceneSystem>()->SetCurrentScene(scene);

            scene->Load(fileName.toStdString());
            SystemManager::Get().GetSystem<ChaosEditor::CameraSystem>()->Enable();
            Events::EventHandler::Get().Send<Events::SceneChangedEvent>(*scene);
        }
    }

    void MenuBar::CreateEmptyGameObject()
    {
        using namespace Chaos;

        int         copies  = 0;
        std::string name    = "Empty" + std::to_string(copies);
        StringID	nameID  = operator""_sid(name.c_str(), name.size());

        while (EntityManager::Get().GetEntity(nameID))
        {
            ++copies;
            name    = "Empty" + std::to_string(copies);
            nameID  = operator""_sid(name.c_str(), name.size());
        }
        EmptyGameObject& emptyGO = SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<EmptyGameObject>(nameID);
        Events::EventHandler::Get().Send<Events::SceneChangedEvent>(*SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene());
    }

    void MenuBar::CreateMaterial()
    {
        using namespace Chaos;
        CHAOS_CORE_TRACE("Create Material");

        int         copies  = 0;
        std::string name    = "New Material" + std::to_string(copies);
        StringID	nameID  = operator""_sid(name.c_str(), name.size());

        while (MaterialManager::Get().GetMaterial(nameID))
        {
            ++copies;
            name    = "New Material" + std::to_string(copies);
            nameID  = operator""_sid(name.c_str(), name.size());
        }

        auto currentScene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
        StringID group = currentScene->GetGroup();
        Resources::MaterialPtr newMaterial = std::reinterpret_pointer_cast<Resources::Material>(MaterialManager::Get().CreateResource(name, group));

        if (s_inspector)
        {
            s_ui->hierarchyView->selectionModel()->clearSelection();
            s_inspector->MaterialSelected(newMaterial);
        }
    }

    void MenuBar::GenerateBRDF()
    {
       std::vector<float> brdfMap = Chaos::Resources::ASkybox::GenerateBRDF();
        Chaos::AssetsManager::AssetsLoader::ExportTexture(brdfMap.data(), "BRDF", 512, 512, 2, Chaos::E_TextureType::RG16F);
    }

    void MenuBar::ImportAsset()
    {
        auto importedAsset = QFileDialog::getOpenFileName(nullptr, "Import Asset", QApplication::applicationDirPath() + "/Assets/", "All files (*.*);;Mesh file (*.fbx *.obj *.3ds);;Images(*.png *.jpeg *.psd)");

        if (!importedAsset.isNull())
        {
            std::string fileType(importedAsset.toStdString().substr(importedAsset.toStdString().find_last_of(".")));

            if (std::strcmp(fileType.c_str(), ".fbx") == 0 ||
                std::strcmp(fileType.c_str(), ".FBX") == 0 ||
                std::strcmp(fileType.c_str(), ".obj") == 0 ||
                std::strcmp(fileType.c_str(), ".3ds") == 0)
            {
				Chaos::AssetsManager::AssetsLoader::VerifyAnimation(importedAsset.toStdString());
            }
            else if (std::strcmp(fileType.c_str(), ".png") == 0 ||
                std::strcmp(fileType.c_str(), ".jpeg") == 0 ||
                std::strcmp(fileType.c_str(), ".jpg") == 0 ||
                std::strcmp(fileType.c_str(), ".tga") == 0 ||
                std::strcmp(fileType.c_str(), ".psd") == 0)
            {
                Chaos::AssetsManager::AssetsLoader::LoadTexture(importedAsset.toStdString());

                std::string name(std::filesystem::path(importedAsset.toStdString()).filename().stem().string());
                if (filesystem::exists("Assets/DefaultResources/Editor.resdecl"))
                {
                    std::ofstream out("Assets/DefaultResources/Editor.resdecl", std::ios::out | std::ios::app);
                    if (out.is_open())
                        out << "\n" + name + ",TEXTURE,Editor";
                    out.close();
                }
            }
            else if (std::strcmp(fileType.c_str(), ".hdr") == 0)
            {
                std::vector<float> textureData;
                int width, height, bpp = 0;
                Chaos::AssetsManager::AssetsLoader::LoadHDRTexture(importedAsset.toStdString(), textureData, width, height, bpp);
                Chaos::Resources::SkyboxData skybox = Chaos::Resources::ASkybox::PrecomputeSkybox(textureData.data(), width, height, bpp, 1024, 512);
                Chaos::AssetsManager::AssetsLoader::ExportSkybox(importedAsset.toStdString(),
                    skybox.environmentMap, 1024,
                    skybox.irradianceMap, 32,
                    skybox.prefilterMap, 512);

                std::string name(std::filesystem::path(importedAsset.toStdString()).filename().stem().string());
                if (filesystem::exists("Assets/DefaultResources/Editor.resdecl"))
                {
                    std::ofstream out("Assets/DefaultResources/Editor.resdecl", std::ios::out | std::ios::app);
                    if (out.is_open())
                        out << "\n" + name + ",SKYBOX,Editor";
                    out.close();
                }
            }
            else if (std::strcmp(fileType.c_str(), ".shader") == 0)
            {
                std::string name(std::filesystem::path(importedAsset.toStdString()).filename().stem().string());
                if (filesystem::exists("Assets/DefaultResources/Editor.resdecl"))
                {
                    std::ofstream out("Assets/DefaultResources/Editor.resdecl", std::ios::out | std::ios::app);
                    if (out.is_open())
                        out << "\n" + name + ",SHADER,Editor";
                    out.close();
                }
            }
            else if (std::strcmp(fileType.c_str(), ".wav") == 0)
            {
                std::string name(std::filesystem::path(importedAsset.toStdString()).filename().stem().string());
                if (filesystem::exists("Assets/DefaultResources/Editor.resdecl"))
                {
                    std::ofstream out("Assets/DefaultResources/Editor.resdecl", std::ios::out | std::ios::app);
                    if (out.is_open())
                        out << "\n" + name + ",AUDIO,Editor";
                    out.close();
                }
            }
        }
    }

}