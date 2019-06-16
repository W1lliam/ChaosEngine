
#include "ChaosEditor/LevelEditor.h"
#include <ChaosEngine.h>
#include <qapplication.h>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QDragMoveEvent>
#include <QMimeData>
#include <ChaosGraphics/Renderer/OpenGLRenderer.h>
#include "ChaosEditor/CameraSystem.h"
#include "ChaosEditor/editor.h"
#include "ChaosEditor/InputHandler.h"
#include "ChaosEditor/MenuBar.h"
#include "ChaosEditor/Inspector/Inspector.h"
#include "ui_editor.h"
#include <random>

bool LevelEditor::s_gladInitialized = false;

LevelEditor::LevelEditor(QWidget *parent) : QOpenGLWidget(parent)
{
	using namespace Chaos;
	RegisterEventCallback<Events::SceneChangedEvent>(std::bind(&LevelEditor::OnSceneChanged, this, std::placeholders::_1));
}

LevelEditor::~LevelEditor()
{
	using namespace Chaos;
	UnregisterAllEventCallbacks();
}

void LevelEditor::initializeGL()
{
	using namespace Chaos;
	if (!s_gladInitialized)
	{
		s_gladInitialized = true;
	}
	
    gladLoadGL();
	Graphics::IRenderer::Init();
	Graphics::IRenderer::Get().SetResizeMethod(Graphics::E_ResizeMethod::FILL);	

    ResourceGroupManager::Get().LoadResourceGroup("DefaultResources"_sid);
    ResourceGroupManager::Get().LoadResourceGroup("DefaultSkies"_sid);
    ResourceGroupManager::Get().LoadResourceGroup("Editor"_sid);
    //ResourceGroupManager::Get().LoadResourceGroup("Audio"_sid);
  //  ResourceGroupManager::Get().LoadResourceGroup("Globals"_sid);
   // ResourceGroupManager::Get().LoadResourceGroup("Scene1"_sid);
    //ResourceGroupManager::Get().LoadResourceGroup("Scene2"_sid);

	//mainCamera = &SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<CameraGameObject>("MainCamera"_sid);
	//mainCamera->GetComponent<TransformComponent>()->AsTransform().SetPosition(0.f, 2.f, 3.f);

	//m_gizmo.SetCameraPosition(mainCamera->GetComponent<TransformComponent>()->AsTransform());

	//auto camSys         = SystemManager::Get().AddSystem<CameraSystem>(mainCamera->GetEntityID());
	//auto lightSys       = SystemManager::Get().AddSystem<LightSystem>();
	//auto renderSys      = SystemManager::Get().AddSystem<RenderSystem>();


    //auto scene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
    //scene->AddSystem(camSys);
    //scene->AddSystem(lightSys);
    //scene->AddSystem(renderSys);


    //Resources::MaterialPtr gridMtl = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("GridMaterial", "Editor"_sid).first);
    //gridMtl->SetShader(ShaderManager::Get().GetShader("grid"_sid, "Editor"_sid));

	//Resources::MeshPtr      cubeMesh        = MeshManager::Get().GetMesh		("Cube"_sid,            "DefaultResources"_sid);
	//Resources::MeshPtr      sphereMesh      = MeshManager::Get().GetMesh		("Sphere"_sid,          "DefaultResources"_sid);
	//Resources::MeshPtr      planeMesh       = MeshManager::Get().GetMesh		("Plane"_sid,           "DefaultResources"_sid);
 //   Resources::MeshPtr      droneMesh       = MeshManager::Get().GetMesh        ("drone_final_low2"_sid,"Scene2"_sid          );
	//Resources::ShaderPtr    shader          = ShaderManager::Get().GetShader	("basic"_sid,	        "DefaultResources"_sid);
  /*  Resources::ShaderPtr    gshader         = ShaderManager::Get().GetShader    ("gbuffer"_sid,         "DefaultResources"_sid);
    Resources::ShaderPtr    gshadermap      = ShaderManager::Get().GetShader    ("gbuffermaps"_sid,		"DefaultResources"_sid);

	Resources::TexturePtr   missingTexture  = TextureManager::Get().GetTexture	("MissingTexture"_sid,  "DefaultResources"_sid);
    Resources::MaterialPtr	defaultMtl      = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("Default", "DefaultResources"_sid).first);
    defaultMtl->SetShader(gshader);
    defaultMtl->SetTexture("uTexture", missingTexture);

	Resources::TexturePtr   ceramicTexture  = TextureManager::Get().GetTexture("Ceramic"_sid,   "Scene2"_sid);
    Resources::MaterialPtr	basicMtl        = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("Basic", "DefaultResources"_sid).first);
    basicMtl->SetShader(gshader);
    basicMtl->SetTexture("uTexture", ceramicTexture);


    Resources::TexturePtr   marble = TextureManager::Get().GetTexture("Marble"_sid, "Scene2"_sid);
    Resources::MaterialPtr	planeMtl = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("Plane", "Scene2"_sid).first);
    planeMtl->SetShader(gshader);
    planeMtl->SetTexture("uTexture", marble);


    Resources::TexturePtr   gunAlbedo = TextureManager::Get().GetTexture("Revolver_AAO"_sid, "Globals"_sid);
    Resources::TexturePtr   gunNormal = TextureManager::Get().GetTexture("Revolver_N"_sid, "Globals"_sid);
    Resources::TexturePtr   gunMetallic = TextureManager::Get().GetTexture("Revolver_M"_sid, "Globals"_sid);
    Resources::TexturePtr   gunRoughness = TextureManager::Get().GetTexture("Revolver_R"_sid, "Globals"_sid);
    Resources::TexturePtr   gunEmissive = TextureManager::Get().GetTexture("Revolver_E"_sid, "Globals"_sid);
    Resources::MaterialPtr	gunMat = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("Revolver_Material", "Globals"_sid).first);
    gunMat->SetShader(gshadermap);
    gunMat->SetTexture("uTexture", gunAlbedo);
    gunMat->SetTexture("uNormal", gunNormal);
    gunMat->SetTexture("uMetallic", gunMetallic);
    gunMat->SetTexture("uRoughness", gunRoughness);
    gunMat->SetTexture("uEmissive", gunEmissive);*/


    //Resources::MeshPtr		shieldMesh       = MeshManager::Get().GetMesh        ("ShieldMesh"_sid,			"Globals"_sid          );
 //   Resources::TexturePtr   shieldAlbedo = TextureManager::Get().GetTexture("RedWhiteShield_AAO"_sid, "Globals"_sid);
 //   Resources::TexturePtr   shieldNormal = TextureManager::Get().GetTexture("RedWhiteShield_N"_sid, "Globals"_sid);
 //   Resources::TexturePtr   shieldMetallic = TextureManager::Get().GetTexture("RedWhiteShield_M"_sid, "Globals"_sid);
 //   Resources::TexturePtr   shieldRoughness = TextureManager::Get().GetTexture("RedWhiteShield_R"_sid, "Globals"_sid);
 //   Resources::TexturePtr   shieldEmissive = TextureManager::Get().GetTexture("RedWhiteShield_E"_sid, "Globals"_sid);
 //   Resources::MaterialPtr	shieldMat = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("Shield_Material", "Globals"_sid).first);
 //   shieldMat->SetShader(gshadermap);
 //   shieldMat->SetTexture("uTexture",	shieldAlbedo);
 //   shieldMat->SetTexture("uNormal",	shieldNormal);
 //   shieldMat->SetTexture("uMetallic",	shieldMetallic);
 //   shieldMat->SetTexture("uRoughness", shieldRoughness);
 //   shieldMat->SetTexture("uEmissive",	shieldEmissive);

	////Resources::MeshPtr		cerberusMesh       = MeshManager::Get().GetMesh		("Cerberus_LP"_sid, "Globals"_sid          );
	//Resources::TexturePtr   cerberusAlbedo = TextureManager::Get().GetTexture	("Cerberus_A"_sid,	"Globals"_sid);
	//Resources::TexturePtr   cerberusNormal = TextureManager::Get().GetTexture	("Cerberus_N"_sid,	"Globals"_sid);
	//Resources::TexturePtr   cerberusMetallic = TextureManager::Get().GetTexture	("Cerberus_M"_sid,	"Globals"_sid);
	//Resources::TexturePtr   cerberusRoughness = TextureManager::Get().GetTexture("Cerberus_R"_sid,	"Globals"_sid);
	//Resources::MaterialPtr	cerberusMat = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("Cerberus_Material", "Globals"_sid).first);
	//cerberusMat->SetShader(gshadermap);
	//cerberusMat->SetTexture("uTexture",		cerberusAlbedo);
	//cerberusMat->SetTexture("uNormal",		cerberusNormal);
	//cerberusMat->SetTexture("uMetallic",	cerberusMetallic);
	//cerberusMat->SetTexture("uRoughness",	cerberusRoughness);
	//cerberusMat->SetTexture("uEmissive",	shieldEmissive);

 //   ScriptedComponent&      scriptComp  = ComponentManager::Get().AddScriptedComponent(cube.GetEntityID(), "PlayerComponent");
 //   MeshGameObject          sphere      = SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<MeshGameObject>("Sphere"_sid,   sphereMesh,   basicMtl);
	//MeshGameObject          sphere2     = SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<MeshGameObject>("Sphere2"_sid,  sphereMesh,   basicMtl);



	//LightGameObject	light   = SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<LightGameObject>("MainLight"_sid);
	//LightGameObject	light2  = SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<LightGameObject>("RimLight"_sid);
	//LightGameObject	light3  = SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<LightGameObject>("FillLight"_sid);




   // cube.GetComponent<TransformComponent>()->AsTransform().SetPosition(0, 0, 0);

    //Resources::AudioClipPtr     clip = std::reinterpret_pointer_cast<Resources::AudioClip>(AudioResourcesManager::Get().CreateOrRetrieve("ha", "Audio"_sid).first);
    //AudioSourceComponent&		audio = cube.AddComponent<AudioSourceComponent>(clip);
    //audio.SetLoopingMode(true);
    //audio.SetSpaceMode(true);
    //audio.SetVolume(1.0);
    //audio.Play();
	//sphere.GetComponent<TransformComponent>()->AsTransform().SetPosition(5, 5, 0);
	//sphere2.GetComponent<TransformComponent>()->AsTransform().SetPosition(0, 5, 5);

	/*light.GetComponent<LightComponent>()->SetIntensity(0.7f);
	light.GetComponent<TransformComponent>()->AsTransform().SetPosition(0.0f, 1.0f, 2.0f);
	light.GetComponent<TransformComponent>()->AsTransform().SetOrientation(-28.0f, -64.776f, 0.0f);
	light.GetComponent<LightComponent>()->SetRange(50.0f);
	light.GetComponent<LightComponent>()->SetType(E_LightType::DIRECTIONAL);

	light2.GetComponent<TransformComponent>()->AsTransform().SetPosition(0.0f, 2.0f, -2.0f);
	light2.GetComponent<TransformComponent>()->AsTransform().SetOrientation(0.0f, 180.0f, 0.0f);
	light2.GetComponent<LightComponent>()->SetRange(50.0f);
	light2.GetComponent<LightComponent>()->SetIntensity(2.0f);
	light2.GetComponent<LightComponent>()->SetAngle(65.0f);
	light2.GetComponent<LightComponent>()->SetColor(Maths::Vector4(0.039f, 0.788f, 1.0f, 1.0f));
	light2.GetComponent<LightComponent>()->SetType(E_LightType::SPOT);

	light3.GetComponent<TransformComponent>()->AsTransform().SetPosition(2.228f, -0.37f, 1.259f);
	light3.GetComponent<TransformComponent>()->AsTransform().SetOrientation(10.561f, 14.263f, 0.0f);
	light3.GetComponent<LightComponent>()->SetRange(50.0f);
	light3.GetComponent<LightComponent>()->SetIntensity(0.2f);
	light3.GetComponent<LightComponent>()->SetType(E_LightType::SPOT);



	scene->SetParent(sphere.GetEntityID(), cube.GetEntityID());
	scene->SetParent(sphere2.GetEntityID(), sphere.GetEntityID());


    droneMtl->SetShader(gshader);
    droneMtl->SetTexture("uTexture", droneAlbedo);
    droneMtl->SetUniform("emissive", Maths::Vector3(0.0f, 0.0f, 0.0f));
	Resources::MeshPtr      testMesh    = MeshManager::Get().GetMesh("DickButt_mesh"_sid, "Scene2"_sid);
	Resources::TexturePtr   special     = TextureManager::Get().GetTexture("dickbutt"_sid, "Scene2"_sid);
	Resources::MaterialPtr	planeMtl    = std::static_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve("Plane", "Scene2"_sid).first);*/

	/*auto scene2 = std::reinterpret_pointer_cast<Scene>(SceneManager::Get().CreateOrRetrieve("EditorScene2", "Scene2"_sid).first);
    scene2->GetPhysicsScene().AddPlane(Maths::Vector3(0.f, 1.f, 0.f), Physics::Friction(1.0f, 1.0f, 1.0f), 0.f);
	SystemManager::Get().GetSystem<SceneSystem>()->SetCurrentScene(scene2);

	//MeshGameObject  plane		= SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<MeshGameObject>("plane"_sid, planeMesh, planeMtl);
	//MeshGameObject  EasterEgg	= SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<MeshGameObject>("EasterEgg"_sid, droneMesh, basicMtl);
    //LightGameObject	scene2light = SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<LightGameObject>("MainLight2"_sid);

    scene2light.GetComponent<LightComponent>()->SetIntensity(0.7f);
    scene2light.GetComponent<TransformComponent>()->AsTransform().SetPosition(0.0f, 1.0f, 2.0f);
    scene2light.GetComponent<TransformComponent>()->AsTransform().SetOrientation(-28.0f, -64.776f, 0.0f);
    scene2light.GetComponent<LightComponent>()->SetRange(50.0f);
    scene2light.GetComponent<LightComponent>()->SetType(E_LightType::DIRECTIONAL);*/

	//SystemManager::Get().GetSystem<SceneSystem>()->SetCurrentScene(scene);
    //scene->AddScriptedSystem("PlayerSystem");

    RenderSystem*   renderSys   = SystemManager::Get().AddSystem<RenderSystem>();
    CameraSystem*   cameraSys   = SystemManager::Get().AddSystem<CameraSystem>();
    LightSystem*    lightSys    = SystemManager::Get().AddSystem<LightSystem>();
    AudioSystem*	audioSys    = SystemManager::Get().AddSystem<AudioSystem>();
	AnimationSystem* animSys    = SystemManager::Get().AddSystem<AnimationSystem>();
    ScriptsHandler::Get().CompileComponents();
    ScriptsHandler::Get().CompileSystems();

    SystemManager::Get().AddSystem<SceneSystem>();
    Chaos::SystemManager::Get().AddSystem<ChaosEditor::CameraSystem>();
 
    SystemManager::Get().UpdateSystemWorkOrder();
}

void LevelEditor::resizeGL(int w, int h)
{
	if (w > 400)
	{
		m_width = w;
		m_height = h;
		Chaos::Events::EventHandler::Get().Send<Chaos::Events::WindowResizeEvent>(w, h);
	}
}

void LevelEditor::paintGL()
{
	using namespace Chaos;
	using Clock = std::chrono::high_resolution_clock;

	Graphics::IRenderer::Get().ResizeViewport(m_width, m_height);
    auto editorCameraSys = Chaos::SystemManager::Get().GetSystem<ChaosEditor::CameraSystem>();
    editorCameraSys->Enable();

    if (m_loadSceneNextFrame)
    {
        if (m_frameCountdown > 1)
        {
            --m_frameCountdown;
        }
        else
        {
            auto scene = Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene();
            Chaos::ReadStream readStream(&m_sceneData[0], (uint32_t)m_sceneData.size());
            scene->Load(readStream);
            Chaos::SystemManager::Get().GetSystem<ChaosEditor::CameraSystem>()->Enable();
            Chaos::Events::EventHandler::Get().Send<Chaos::Events::SceneChangedEvent>(*scene);
            m_loadSceneNextFrame = false;
            m_frameCountdown = 3;
        }
        ChaosEditor::MenuBar::s_inspector->EntitySelected(ChaosEditor::MenuBar::s_ui->hierarchyView->model()->index(0, 0));
    }

	if (!m_gizmo.HasCamera())
	{
        mainCamera = editorCameraSys->GetMainCameraGameObject();
		m_gizmo.SetCameraPosition(mainCamera->GetComponent<TransformComponent>()->AsTransform());
	}


	Utils::Timer::Frame();
	// FixedUpdate
	while (Utils::Timer::Accumulator() >= Utils::Timer::FIXED_DELTA_TIME)
	{
		Utils::Timer::FixedUpdate();
		Chaos::FixedUpdate(Utils::Timer::FIXED_DELTA_TIME.count());
	}

    auto renderSys = SystemManager::Get().GetSystem<RenderSystem>();
    if(renderSys)
        renderSys->SetFinalOutputBuffer(defaultFramebufferObject());

	Chaos::Update(Utils::Timer::DeltaTime().count());
	m_gizmo.Draw();
	update();
}

void LevelEditor::LoadSceneNextFrame(std::vector<uint8_t>& p_sceneData)
{
    m_sceneData = p_sceneData;
    m_loadSceneNextFrame = true;
}

void LevelEditor::Unselect()
{
	m_gizmo.SetGizmoType(E_GizmoType::NONE);
}

void LevelEditor::OnMoveActionTrigger()
{
	m_gizmo.SetGizmoType(E_GizmoType::MOVE);
}

void LevelEditor::OnRotateActionTrigger()
{
	m_gizmo.SetGizmoType(E_GizmoType::ROTATE);
}

void LevelEditor::OnScaleActionTrigger()
{
	m_gizmo.SetGizmoType(E_GizmoType::SCALE);
}

void LevelEditor::mousePressEvent(QMouseEvent *event)
{
	Chaos::Events::EventHandler::Get().Send<Chaos::Events::MouseButtonPressedEvent>(InputHandler::QtToChaosMouseCode(event->button()));
	InputHandler::mouseInput[event->button()] = true;
	QOpenGLWidget::mousePressEvent(event);
}

void LevelEditor::mouseReleaseEvent(QMouseEvent * event)
{
    Chaos::Events::EventHandler::Get().Send<Chaos::Events::MouseButtonReleasedEvent>(InputHandler::QtToChaosMouseCode(event->button()));
	InputHandler::mouseInput[event->button()] = false;

	QOpenGLWidget::mouseReleaseEvent(event);
}

void LevelEditor::mouseMoveEvent(QMouseEvent * event)
{
	Chaos::Events::EventHandler::Get().Send<Chaos::Events::MouseMovedEvent>(event->x(), event->y());
	QOpenGLWidget::mouseMoveEvent(event);
}

void LevelEditor::mouseDoubleClickEvent(QMouseEvent * event)
{
	QOpenGLWidget::mouseDoubleClickEvent(event);
}

void LevelEditor::wheelEvent(QWheelEvent * event)
{
	QOpenGLWidget::wheelEvent(event);
}

void LevelEditor::keyPressEvent(QKeyEvent* event)
{
	using namespace Chaos;
	
	Events::EventHandler::Get().Send<Chaos::Events::KeyPressedEvent>(InputHandler::QtToChaosKeyCode((Qt::Key)event->key()), 0);

    if ((Qt::Key)event->key() == Qt::Key::Key_Z)
        m_gizmo.ToggleGizmoLocal();

	InputHandler::keyInput[(Qt::Key)event->key()] = true;

	QOpenGLWidget::keyPressEvent(event);
}

void LevelEditor::keyReleaseEvent(QKeyEvent * event)
{
	Chaos::Events::EventHandler::Get().Send<Chaos::Events::KeyReleasedEvent>(InputHandler::QtToChaosKeyCode((Qt::Key)event->key()));

	InputHandler::keyInput[(Qt::Key)event->key()] = false;

	QOpenGLWidget::keyReleaseEvent(event);
}

void LevelEditor::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/plain"))
		event->acceptProposedAction();
}

void LevelEditor::EntitySelected(QModelIndex p_index)
{
	if (p_index.parent().isValid())
	{
		Chaos::EntityPtr	entity = static_cast<Chaos::Scene::SceneNode*>(p_index.internalPointer())->entity;
		m_gizmo.SetSelected(entity);
	}
	else
	{
		m_gizmo.SetSelected(nullptr);
	}
}

bool LevelEditor::OnSceneChanged(const Chaos::Events::SceneChangedEvent & e)
{
	return false;
}
