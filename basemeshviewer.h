#ifndef BASEMESHVIEWER_H
#define BASEMESHVIEWER_H
#include <GL\glew.h>
#include "Shader\ShaderManager.h"
#include <QtOpenGL>
#include "Camera\camera.h"
#include "event_handles\AbstractMeshEventHandle.h"
class BaseMeshViewer : public QGLWidget
{
	Q_OBJECT

public:
	BaseMeshViewer(QWidget *parent);
	~BaseMeshViewer();

	const Camera& camera()const{ return m_camera; }
	Camera& camera(){ return m_camera; }
	void resetCamera();
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void toggleShowType(int t){ if (m_showType & t) m_showType -= t; else m_showType |= t; }
	int getShowType()const{ return m_showType; }
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
protected:
	AbstractMeshEventHandle* m_currentEventHandle;
	std::vector<std::shared_ptr<AbstractMeshEventHandle>> m_eventHandles;
};

#endif // BASEMESHVIEWER_H
