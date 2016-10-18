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
	enum {
		TrackBallIndex_X = 1,
		TrackBallIndex_Y,
		TrackBallIndex_Z,
	};
public:
	BaseMeshViewer(QWidget *parent);
	~BaseMeshViewer();

	void initCloth(CAnalysis2D_Cloth_Static* pAnalysis, CDesigner2D_Cloth* pListener);
	CAnalysis2D_Cloth_Static* pAnalysis() { return m_pAnalysis; }
	void getModelBound(ldp::Float3& bmin, ldp::Float3& bmax);
	const CAnalysis2D_Cloth_Static* pAnalysis()const { return m_pAnalysis; }
	CDesigner2D_Cloth* pListener() { return m_pListener; }
	const CDesigner2D_Cloth* pListener()const { return m_pListener; }

	float getFps()const{ return m_fps; }
	bool isEdgeMode()const{ return m_isEdgeMode; }
	void setEdgeMode(bool b){ m_isEdgeMode = b; }
	const ldp::Camera& camera()const{ return m_camera; }
	ldp::Camera& camera(){ return m_camera; }
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
	void rotateTrackBall(ldp::Mat3d R);
	void endDragBox();
	void beginTrackBall(ldp::Float3 p, ldp::Mat3f R, float scale);
	void endTrackBall();
	bool isTrackBallShown()const{ return m_isTrackBall; }
	void setActiveTrackBallAxis(int i){ m_activeTrackBallAxis = i; }
	int getActiveTrackBallAxis()const{ return m_activeTrackBallAxis; }
	void setHoverTrackBallAxis(int i){ m_hoverTrackBallAxis = i; }
	int getHoverTrackBallAxis()const{ return m_hoverTrackBallAxis; }

	int fboRenderedIndex(QPoint p)const;
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent*);
	void mouseDoubleClickEvent(QMouseEvent *ev);
	void wheelEvent(QWheelEvent*);
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);
	void renderSelectionOnFbo();
	void renderDragBox();
	void renderTrackBall(bool idxMode);
	void timerEvent(QTimerEvent* ev);
protected:
	ldp::Camera m_camera;
	QPoint m_lastPos;
	int m_showType;
	Qt::MouseButtons m_buttons;
	CShaderManager m_shaderManager;
	QGLFramebufferObject* m_fbo;
	QImage m_fboImage;
	bool m_isDragBox;
	QPoint m_dragBoxBegin;
	bool m_isTrackBall;
	ldp::Float3 m_trackBallPos;
	ldp::Mat3f m_trackBallR;
	float m_trackBallScale;
	int m_activeTrackBallAxis;
	int m_hoverTrackBallAxis;
	AbstractMeshEventHandle* m_currentEventHandle;
	std::vector<std::shared_ptr<AbstractMeshEventHandle>> m_eventHandles;

	CAnalysis2D_Cloth_Static* m_pAnalysis;
	CDesigner2D_Cloth* m_pListener;
	ldp::Float3 m_modelBound[2];
	int m_computeTimer, m_renderTimer;
	float m_fps;
	bool m_isEdgeMode;
};

