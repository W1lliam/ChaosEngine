#include "ChaosAssetsManager.h"
#include "ChaosEditor/editor.h"
#include "ui_editor.h"
#include <iostream>
#include <qfiledialog.h>

#include <QKeyEvent>

#include <QToolbar>
#include <ChaosEngine.h>
#include <QOpenGLContext>
#include <QDockWidget>

#include "ChaosEditor/Inspector/Inspector.h"
#include "ChaosEditor/CameraSystem.h"
#include "ChaosEditor/Hierarchy.h"
#include "ChaosEditor/MenuBar.h"

#include <QDrag>
#include <QMimeData>

Editor::Editor(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::Editor)
{
	setDockOptions(DockOption::AllowNestedDocks | DockOption::AnimatedDocks | DockOption::AllowTabbedDocks);

	SetupResourceGroups();
    
	RegisterEventCallback<Chaos::Events::MouseMovedEvent>(std::bind(&Editor::OnEvent, this, std::placeholders::_1));
	RegisterEventCallback<Chaos::Events::KeyPressedEvent>(std::bind(&Editor::OnEvent, this, std::placeholders::_1));
	RegisterEventCallback<Chaos::Events::MouseButtonPressedEvent>(std::bind(&Editor::OnEvent, this, std::placeholders::_1));

    m_ui->setupUi(this);


	CreateLevelEditor();
	CreateToolbar();

	CreateInspector();
	CreateHierarchy();
	CreateFileExplorer();
	CreateConsole();

    CreateMenuBar();

	QObject::connect(m_ui->actionOpen,			SIGNAL(triggered()), this,	SLOT(OnOpenActionTrigger()));
}

Editor::~Editor()
{
	delete m_ui;
	delete m_glWindow;

	delete m_toolbar;
	delete m_menuBar;

	delete m_hierarchy;
	delete m_explorer;
	delete m_inspector;



	delete dirmodel;
	delete filemodel;
}

void Editor::CreateInspector()
{
    m_inspector = new ChaosEditor::Inspector(m_ui->inspector, this);

    addDockWidget(Qt::RightDockWidgetArea, m_inspector);
}

void Editor::CreateHierarchy()
{
	m_hierarchy = new QDockWidget("HIERARCHY", this);
	m_hierarchy->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);

	m_hierarchy->setWidget(m_ui->hierarchyView);
	m_hierarchy->setAttribute(Qt::WA_WState_ExplicitShowHide);
	addDockWidget(Qt::LeftDockWidgetArea, m_hierarchy);
    Hierarchy* hierarchy = new Hierarchy(this);
    m_ui->hierarchyView->setModel(hierarchy);
    m_ui->hierarchyView->setAlternatingRowColors(false);

	m_ui->hierarchyView->setDragDropMode(QAbstractItemView::InternalMove);
	m_ui->hierarchyView->setDragEnabled(true);
	m_ui->hierarchyView->setAcceptDrops(true);
	m_ui->hierarchyView->setDropIndicatorShown(true);

    QObject::connect(   m_ui->hierarchyView,    SIGNAL(clicked(QModelIndex)),
                        m_inspector,            SLOT(EntitySelected(QModelIndex)));

	QObject::connect(	m_ui->hierarchyView,	SIGNAL(clicked(QModelIndex)),
						m_glWindow,				SLOT(EntitySelected(QModelIndex)));

	QObject::connect(   hierarchy,		        SIGNAL(modelReset()), 
                        this,		            SLOT(OnHierarchyReset()));

	m_ui->hierarchyView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(m_ui->hierarchyView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowHierarchyMenu(const QPoint&)));
}

void Editor::CreateLevelEditor()
{
	m_glWindow = new LevelEditor(this);
	m_glWindow->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

	m_ui->splitter->setStretchFactor(0, 0);
	m_ui->splitter->setStretchFactor(1, 5);
	m_ui->splitter->setStretchFactor(2, 0);
    
	m_ui->mainFrameSplitter->addWidget(m_glWindow);

}

void Editor::CreateToolbar()
{
	m_toolbar = addToolBar("Tools");

    const char* iconColor = "light";

    QString iconsPath = QApplication::applicationDirPath() + "/EditorResources/icons";
    
    QIcon moveIcon;
    moveIcon.addFile(iconsPath + "/64x64/move_" + iconColor + "_64.png", QSize(64, 64), QIcon::Normal, QIcon::Off);
    moveIcon.addFile(iconsPath + "/64x64/move_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Active, QIcon::Off);
    moveIcon.addFile(iconsPath + "/64x64/move_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Normal, QIcon::On);
    QIcon rotateIcon;
    rotateIcon.addFile(iconsPath + "/64x64/rotate_" + iconColor + "_64.png", QSize(64, 64), QIcon::Normal, QIcon::Off);
    rotateIcon.addFile(iconsPath + "/64x64/rotate_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Active, QIcon::Off);
    rotateIcon.addFile(iconsPath + "/64x64/rotate_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Normal, QIcon::On);
    QIcon scaleIcon;
    scaleIcon.addFile(iconsPath + "/64x64/scale_" + iconColor + "_64.png", QSize(64, 64), QIcon::Normal, QIcon::Off);
    scaleIcon.addFile(iconsPath + "/64x64/scale_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Active, QIcon::Off);
    scaleIcon.addFile(iconsPath + "/64x64/scale_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Normal, QIcon::On);
    
    QIcon compileIcon;
    compileIcon.addFile(iconsPath + "/64x64/compile_" + iconColor + "_64.png", QSize(64, 64), QIcon::Normal, QIcon::Off);
    compileIcon.addFile(iconsPath + "/64x64/compile_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Active, QIcon::Off);
    compileIcon.addFile(iconsPath + "/64x64/compile_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Normal, QIcon::On);
    QIcon compileShadersIcon;
    compileShadersIcon.addFile(iconsPath + "/64x64/compile_shaders_" + iconColor + "_64.png", QSize(64, 64), QIcon::Normal, QIcon::Off);
    compileShadersIcon.addFile(iconsPath + "/64x64/compile_shaders_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Active, QIcon::Off);
    compileShadersIcon.addFile(iconsPath + "/64x64/compile_shaders_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Normal, QIcon::On);
    QIcon playIcon;
    playIcon.addFile(iconsPath + "/64x64/play_" + iconColor + "_64.png", QSize(64, 64), QIcon::Normal, QIcon::Off);
    playIcon.addFile(iconsPath + "/64x64/play_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Active, QIcon::Off);
    playIcon.addFile(iconsPath + "/64x64/play_" + iconColor + "_hov_64.png", QSize(64, 64), QIcon::Normal, QIcon::On);

	QAction* move = new QAction(moveIcon, "Move");
	move->setCheckable(true);
	m_toolbar->addAction(move);
	QAction* rotate = new QAction(rotateIcon, "Rotate");
	rotate->setCheckable(true);
	m_toolbar->addAction(rotate);
    QAction* scale = new QAction(scaleIcon, "Scale");
    scale->setCheckable(true);
    m_toolbar->addAction(scale);
    m_toolbar->addSeparator();

    QAction* compile = new QAction(compileIcon, "Compile Scripts");
    compile->setCheckable(false);
    m_toolbar->addAction(compile);

    QAction* compileShaders = new QAction(compileShadersIcon, "Compile Shaders");
    compileShaders->setCheckable(false);
    m_toolbar->addAction(compileShaders);

	QAction* play = new QAction(playIcon, "Play");
    play->setCheckable(true);
	m_toolbar->addAction(play);
    


	QActionGroup* group = new QActionGroup(m_toolbar);
	group->addAction(move);
	group->addAction(rotate);
	group->addAction(scale);
	group->setExclusive(true);


    QObject::connect(move,		        SIGNAL(triggered()), this,	SLOT(OnMoveActionTrigger()));
    QObject::connect(rotate,	        SIGNAL(triggered()), this,	SLOT(OnRotateActionTrigger()));
    QObject::connect(scale,		        SIGNAL(triggered()), this,	SLOT(OnScaleActionTrigger()));
    QObject::connect(compile,           SIGNAL(triggered()), this,	SLOT(OnCompileActionTrigger()));
    QObject::connect(play,		        SIGNAL(triggered()), this,	SLOT(OnPlayActionTrigger()));
    QObject::connect(compileShaders,    SIGNAL(triggered()), this,	SLOT(OnCompileShaderActionTrigger()));
}

void Editor::CreateFileExplorer()
{
	m_explorer = new QDockWidget("EXPLORER", this);
	m_explorer->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);

	m_explorer->setWidget(m_ui->fileExplorerSplitter);
	m_explorer->setAttribute(Qt::WA_WState_ExplicitShowHide);
	addDockWidget(Qt::LeftDockWidgetArea, m_explorer);

	QString mPath = QApplication::applicationDirPath() + "/Assets/";

	dirmodel = new QFileSystemModel(this);

	QModelIndex indx = dirmodel->index(mPath);
	dirmodel->setRootPath(mPath);
	dirmodel->setReadOnly(true);
	m_ui->folderView->setHeaderHidden(true);
	m_ui->folderView->setModel(dirmodel);
	m_ui->folderView->hideColumn(1);
	m_ui->folderView->hideColumn(2);
	m_ui->folderView->hideColumn(3);
	m_ui->folderView->setRootIndex(indx);
	dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

	filemodel = new QFileSystemModel(this);

	QModelIndex indx1 = filemodel->index(mPath);
	filemodel->setRootPath(mPath);
	m_ui->fileView->setModel(filemodel);
	m_ui->fileView->hideColumn(1);
	m_ui->fileView->hideColumn(2);
	m_ui->fileView->hideColumn(3);
	m_ui->fileView->setRootIndex(indx1);
	filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::NoSymLinks);

	filemodel->setNameFilters(QStringList() << "*.pns" << "*.shader");
	filemodel->setNameFilterDisables(false);
	m_ui->fileView->setHeaderHidden(true);

	QObject::connect(m_ui->folderView, &QAbstractItemView::clicked,
		this, &Editor::OnFolderTreeViewClicked);
	QObject::connect(m_ui->fileView, &QAbstractItemView::clicked,
		this, &Editor::OnFileTreeViewClicked);
}

void Editor::CreateConsole()
{
	m_console = new QDockWidget("CONSOLE", this);
	m_console->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
	console = new Console(this);
	m_console->setWidget(console);
	m_console->setAttribute(Qt::WA_WState_ExplicitShowHide);
	addDockWidget(Qt::LeftDockWidgetArea, m_console);
	console->setReadOnly(true);
}

void Editor::CreateMenuBar()
{
    m_menuBar = new ChaosEditor::MenuBar(m_ui);
    ChaosEditor::MenuBar::BindInspector(static_cast<ChaosEditor::Inspector*>(m_inspector));
    QAction*    toggleToolbar   = m_toolbar->toggleViewAction();
    QAction*    toggleHierarchy = m_hierarchy->toggleViewAction();
    QAction*    toggleExplorer  = m_explorer->toggleViewAction();
    QAction*    toggleInspector = m_inspector->toggleViewAction();
    QAction*    toggleConsole   = m_console->toggleViewAction();

    QMenu*      windowMenu      = m_ui->menubar->addMenu("&Window");
    QMenu*      showMenu        = windowMenu->addMenu("Show");
    showMenu->addActions({ toggleToolbar, toggleHierarchy, toggleExplorer, toggleInspector,toggleConsole });
}

void Editor::OnMoveActionTrigger()
{
	CHAOS_TRACE("Moving");
	m_glWindow->OnMoveActionTrigger();
}

void Editor::OnRotateActionTrigger()
{
	CHAOS_TRACE("Rotating");
	m_glWindow->OnRotateActionTrigger();
}

void Editor::OnScaleActionTrigger()
{
	CHAOS_TRACE("Scaling");
	m_glWindow->OnScaleActionTrigger();
}

void Editor::OnCompileActionTrigger()
{
    Chaos::ScriptsHandler::Get().CompileComponents();
    Chaos::ScriptsHandler::Get().CompileSystems();
}

void Editor::OnPlayActionTrigger()
{
    Chaos::SystemManager& systemMgr = Chaos::SystemManager::Get();
    Chaos::ScriptsHandler::Get().CompileComponents();
    if (!systemMgr.IsInPlayMode())
    {
        Chaos::WriteStream writeStream(1024);
        systemMgr.GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->Save(writeStream);
        writeStream.Finish();

        m_playSceneBuffer.resize(writeStream.GetBytesProcessed());
        memcpy(&m_playSceneBuffer[0], writeStream.GetData(), writeStream.GetBytesProcessed());

        systemMgr.DisableAllSystems();
        systemMgr.GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->EnableSystems();
        systemMgr.Start();
    }
    else
    {
        systemMgr.Stop();
        auto scene = systemMgr.GetSystem<Chaos::SceneSystem>()->GetCurrentScene();
        scene->GetEntities().for_each([](Chaos::Scene::SceneNode& p_node)
        {
            Chaos::EntityManager::Get().DestroyEntity(p_node.entity->GetEntityID());
        });
        scene->Clear();
        m_glWindow->LoadSceneNextFrame(m_playSceneBuffer);
    }
}

void Editor::OnCompileShaderActionTrigger()
{
    Chaos::ShaderManager::Get().ReloadAll();
}


void Editor::OnOpenActionTrigger()
{
	auto selected_file = QFileDialog::getOpenFileName(this, tr("Open file"), QApplication::applicationDirPath() + "/../../../../ProcessedAssets/Assets");

	if (!selected_file.isNull())
	{
		CHAOS_TRACE(selected_file.toStdString());
	}
}

bool Editor::OnEvent(const Chaos::Events::AEvent& e)
{

	return false;
}

void Editor::OnHierarchyReset()
{
	QModelIndexList indexes = m_ui->hierarchyView->model()->match(m_ui->hierarchyView->model()->index(0, 0), Qt::DisplayRole, "*", -1, Qt::MatchWildcard | Qt::MatchRecursive);
	for (QModelIndex& index : indexes)
	{
		m_ui->hierarchyView->expand(index);
	}
}

void Editor::ShowHierarchyMenu(const QPoint& p_pos)
{
	QPersistentModelIndex nd = m_ui->hierarchyView->indexAt(p_pos);
	m_ui->hierarchyView->selectionModel()->select(nd, QItemSelectionModel::SelectCurrent);
	QPoint globalPos = mapToGlobal(p_pos);

	QMenu menu;
	QAction* deleteAct = new QAction("Delete");	
	menu.addAction(deleteAct);
	connect(deleteAct, &QAction::triggered, this, [&]()
	{
		Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->RemoveEntity(static_cast<Chaos::Scene::SceneNode*>(nd.internalPointer())->entity);
		reinterpret_cast<ChaosEditor::Inspector*>(m_inspector)->EntitySelected(m_ui->hierarchyView->model()->index(0, 0));
		Chaos::Events::EventHandler::Get().Send<Chaos::Events::SceneChangedEvent>(*Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene());

	});

	QAction* copyAct = new QAction("Copy");
	menu.addAction(copyAct);
	connect(copyAct, &QAction::triggered, this, [&]()
	{
		CHAOS_CORE_ERROR("Object Copied Name: {0}", static_cast<Chaos::Scene::SceneNode*>(nd.internalPointer())->entity->GetFullName());
	});
	QAction* pasteAct = new QAction("Paste");
	menu.addAction(pasteAct);
	if(!entityClipboard)
		pasteAct->setDisabled(true);
	else 
		pasteAct->setDisabled(false);
	connect(pasteAct, &QAction::triggered, this, [&]()
	{
		CHAOS_CORE_ERROR("Object Pasted Name: {0}", entityClipboard->GetFullName());
		entityClipboard = nullptr;
	});

	QAction* selectedItem = menu.exec(globalPos);
}

void Editor::OnFolderTreeViewClicked(QModelIndex indx)
{
	QString sPath = dirmodel->fileInfo(indx).absoluteFilePath();
	m_ui->fileView->setRootIndex(filemodel->setRootPath(sPath));

	filemodel->setNameFilters(QStringList() << "*.chaosasset" << "*.shader" << "*.chaosscene");
	filemodel->setNameFilterDisables(false);
	filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::NoSymLinks);
}

void Editor::OnFileTreeViewClicked(QModelIndex indx)
{
	CHAOS_TRACE(filemodel->filter().testFlag(QDir::Files));
}

void Editor::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key::Key_Q && m_canChangeMode)
	{
		m_toolbar->actions()[0]->setChecked(false);
		m_toolbar->actions()[1]->setChecked(false);
		m_toolbar->actions()[2]->setChecked(false);
		m_glWindow->Unselect();
	}

	if (e->key() == Qt::Key::Key_W && m_canChangeMode)
	{
		m_toolbar->actions()[0]->setChecked(true);
		OnMoveActionTrigger();
	}
	if (e->key() == Qt::Key::Key_E && m_canChangeMode)
	{
		m_toolbar->actions()[1]->setChecked(true);
		OnRotateActionTrigger();

	}
	if (e->key() == Qt::Key::Key_R && m_canChangeMode)
	{
		m_toolbar->actions()[2]->setChecked(true);
		OnScaleActionTrigger();
	}
}

void Editor::SetupResourceGroups()
{
	using namespace Chaos;

    ResourceGroupManager::Get().LoadResourceDeclaration("Assets/DefaultResources/Defaults.resdecl");
    ResourceGroupManager::Get().LoadResourceDeclaration("Assets/DefaultResources/Editor.resdecl");
    ResourceGroupManager::Get().InitializeAllResourceGroups();
    auto scene = std::reinterpret_pointer_cast<Scene>(SceneManager::Get().CreateOrRetrieve("DefaultScene", "Editor"_sid).first);
    auto physicsSys = SystemManager::Get().AddSystem<PhysicsSystem>();
    auto sceneSys = SystemManager::Get().AddSystem<SceneSystem>(scene);

    scene->AddSystem(physicsSys);
    scene->AddSystem(sceneSys);

    scene->GetPhysicsScene().AddPlane(Maths::Vector3(0.f, 1.f, 0.f), Physics::Friction(0.5f, 0.6f, 0.7f), 0.f);
}

void Editor::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
		m_canChangeMode = false;
}

void Editor::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() == Qt::RightButton)
		m_canChangeMode = true;
}
