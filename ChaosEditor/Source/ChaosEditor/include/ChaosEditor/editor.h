#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include "LevelEditor.h"
#include <qfilesystemmodel.h>
#include "Console/Console.h"

namespace Ui {
class Editor;
}

namespace ChaosEditor
{
    class MenuBar;
}

class Editor : public QMainWindow, public Chaos::Events::EventListener
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

public slots:
	void OnOpenActionTrigger();
	void OnMoveActionTrigger();
	void OnRotateActionTrigger();
	void OnScaleActionTrigger();
	void OnCompileActionTrigger();
	void OnPlayActionTrigger();
	void OnCompileShaderActionTrigger();
	void OnFolderTreeViewClicked(QModelIndex indx);
	void OnFileTreeViewClicked(QModelIndex indx);
	void OnHierarchyReset();

	void ShowHierarchyMenu(const QPoint&);


protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool OnEvent(const Chaos::Events::AEvent& e);

    void CreateToolbar();
    void CreateLevelEditor();

    void CreateInspector();
    void CreateHierarchy();
    void CreateFileExplorer();
    void CreateConsole();

    void CreateMenuBar();

    void SetupResourceGroups();

private:
    Ui::Editor*		        m_ui;
	LevelEditor*	        m_glWindow;
	LevelEditor*	        m_glWindow1;
	QToolBar*		        m_toolbar;
    ChaosEditor::MenuBar*   m_menuBar;
    
	QFileSystemModel*       dirmodel;
	QFileSystemModel*       filemodel;

	QDockWidget*            m_hierarchy;
	QDockWidget*            m_explorer;
	QDockWidget*            m_inspector;
	QDockWidget*            m_console;

	Console*	            console;
    std::vector<uint8_t>    m_playSceneBuffer   {};
	bool		            m_canChangeMode;

public:
	Chaos::EntityPtr entityClipboard{ nullptr };

};

#endif // EDITOR_H
