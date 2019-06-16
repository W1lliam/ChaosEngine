#pragma once

#include <glad/glad.h>
#include <QOpenGLWidget>
#include <ChaosCore.h>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QModelIndex>

#include "ChaosEditor/Gizmo/Gizmo.h"

namespace Chaos
{
	class CameraGameObject;
}

class LevelEditor : public QOpenGLWidget, public Chaos::Events::EventListener //inherited from protected QOpenGLFunctions to not call f->currentContext()->functions()
{
	Q_OBJECT
public:
	explicit LevelEditor(QWidget *parent = nullptr);
	~LevelEditor();

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

    void LoadSceneNextFrame(std::vector<uint8_t>& p_sceneData);
	void Unselect();

public slots:
	void EntitySelected(QModelIndex p_index);
	void OnMoveActionTrigger();
	void OnRotateActionTrigger();
	void OnScaleActionTrigger();

private:
	bool OnSceneChanged(const Chaos::Events::SceneChangedEvent& e);

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;

	void dragEnterEvent(QDragEnterEvent *event) override;

private:
	static bool s_gladInitialized;

	Chaos::GameObject* mainCamera;

    std::vector<uint8_t>    m_sceneData;
    int                     m_frameCountdown = 3;
    bool                    m_loadSceneNextFrame = false;

	float m_width{ 1089 };
	float m_height{ 882 };

	Gizmo m_gizmo;

};