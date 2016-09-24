#ifndef BASEMESHVIEWER_H
#define BASEMESHVIEWER_H

#include "Shader\ShaderManager.h"
#include <QtOpenGL>
#include "camera.h"
class BaseMeshViewer : public QGLWidget
{
	Q_OBJECT

public:
	enum MeshOperationMode
	{
		EditMode,
		ObjectMode,
		MeshOpModeEnd,
	};
public:
	BaseMeshViewer(QWidget *parent);
	~BaseMeshViewer();

	const Camera& camera()const{ return m_camera; }
	Camera& camera(){ return m_camera; }
	void resetCamera();

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void timerEvent(QTimerEvent *) { update(); }

	void toggleShowType(int t){ if (m_showType & t) m_showType -= t; else m_showType |= t; }
	int getShowType()const{ return m_showType; }
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);
public:
	void toggleMeshOperationMode();
	void setMeshOpMode(MeshOperationMode mode);
	MeshOperationMode getMeshOpMode()const{ return m_meshOperationMode; }
protected:
	Camera m_camera;

	QPoint m_lastPos;
	int m_showType;
	MeshOperationMode m_meshOperationMode;
	Qt::MouseButtons m_buttons;

	// for selection rendering
	QGLFramebufferObject* m_fbo;
	QImage m_fboImage;

	bool m_isBoxMode;
	QPoint m_boxBegin;
	QPoint m_boxEnd;

	CShaderManager m_shaderManager;
};

#endif // BASEMESHVIEWER_H
