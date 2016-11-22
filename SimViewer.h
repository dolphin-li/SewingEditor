#pragma once

#include <GL\glew.h>
#include <QtOpenGL>
#include "Camera\camera.h"
#include "event_handles\AbstractSimEventHandle.h"
class CAnalysis2D_Cloth_Static;
class CDesigner2D_Cloth;
class SimViewer : public QGLWidget
{
	Q_OBJECT

public:
	enum {
		TrackBallIndex_X = 1,
		TrackBallIndex_Y,
		TrackBallIndex_Z,
	};
	enum TrackBallMode{
		TrackBall_None,
		TrackBall_Rot,
		TrackBall_Trans
	};
public:
	SimViewer(QWidget *parent);
	~SimViewer();

	void initCloth();
	void getModelBound(ldp::Float3& bmin, ldp::Float3& bmax);

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
	AbstractSimEventHandle::ProcessorType getEventHandleType()const;
	void setEventHandleType(AbstractSimEventHandle::ProcessorType type);
	const AbstractSimEventHandle* getEventHandle(AbstractSimEventHandle::ProcessorType type)const;
	AbstractSimEventHandle* getEventHandle(AbstractSimEventHandle::ProcessorType type);
	void beginDragBox(QPoint p);
	void rotateTrackBall(ldp::Mat3d R);
	void translateTrackBall(ldp::Double3 t);
	void endDragBox();
	void beginTrackBall(TrackBallMode mode, ldp::Float3 p, ldp::Mat3f R, float scale);
	void endTrackBall();
	TrackBallMode getTrackBallMode()const{ return m_trackBallMode; }
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
	QGLFramebufferObject* m_fbo;
	QImage m_fboImage;
	bool m_isDragBox;
	QPoint m_dragBoxBegin;
	TrackBallMode m_trackBallMode;
	ldp::Float3 m_trackBallPos;
	ldp::Mat3f m_trackBallR;
	float m_trackBallScale;
	int m_activeTrackBallAxis;
	int m_hoverTrackBallAxis;
	AbstractSimEventHandle* m_currentEventHandle;
	std::vector<std::shared_ptr<AbstractSimEventHandle>> m_eventHandles;

	ldp::Float3 m_modelBound[2];
	int m_computeTimer, m_renderTimer;
	float m_fps;
	bool m_isEdgeMode;
};

