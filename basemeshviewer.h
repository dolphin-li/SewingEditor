#pragma once

#include <GL\glew.h>
#include "Shader\ShaderManager.h"
#include <QtOpenGL>
#include "Camera\camera.h"
#include "event_handles\AbstractMeshEventHandle.h"
class CAnalysis2D_Cloth_Static;
class CDesigner2D_Cloth;
class BaseMeshViewer : public QGLWidget
{
	Q_OBJECT

public:
	BaseMeshViewer(QWidget *parent);
	~BaseMeshViewer();

	void initCloth(CAnalysis2D_Cloth_Static* pAnalysis, CDesigner2D_Cloth* pListener);
	CAnalysis2D_Cloth_Static* pAnalysis() { return m_pAnalysis; }
	const CAnalysis2D_Cloth_Static* pAnalysis()const { return m_pAnalysis; }
	CDesigner2D_Cloth* pListener() { return m_pListener; }
	const CDesigner2D_Cloth* pListener()const { return m_pListener; }

	float getFps()const{ return m_fps; }
	const Camera& camera()const{ return m_camera; }
	Camera& camera(){ return m_camera; }
	void resetCamera();
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	Qt::MouseButtons buttons()const{ return m_buttons; }
	QPoint lastMousePos()const{ return m_lastPos; }
	const QImage& fboImage()const{ return m_fboImage; }
	AbstractMeshEventHandle::ProcessorType getEventHandleType()const;
	void setEventHandleType(AbstractMeshEventHandle::ProcessorType type);
	const AbstractMeshEventHandle* getEventHandle(AbstractMeshEventHandle::ProcessorType type)const;
	AbstractMeshEventHandle* getEventHandle(AbstractMeshEventHandle::ProcessorType type);
	void beginDragBox(QPoint p);
	void endDragBox();
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);
	void renderSelectionOnFbo();
	void renderDragBox();
	void timerEvent(QTimerEvent* ev);
protected:
	Camera m_camera;
	QPoint m_lastPos;
	int m_showType;
	Qt::MouseButtons m_buttons;
	CShaderManager m_shaderManager;
	QGLFramebufferObject* m_fbo;
	QImage m_fboImage;
	bool m_isDragBox;
	QPoint m_dragBoxBegin;
	AbstractMeshEventHandle* m_currentEventHandle;
	std::vector<std::shared_ptr<AbstractMeshEventHandle>> m_eventHandles;

	CAnalysis2D_Cloth_Static* m_pAnalysis;
	CDesigner2D_Cloth* m_pListener;
	int m_computeTimer, m_renderTimer;
	float m_fps;
};

