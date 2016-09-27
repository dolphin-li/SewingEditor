#ifndef SvgViewer_H
#define SvgViewer_H

#include <QtOpenGL>
#include "camera.h"
namespace svg{
	class SvgManager;
}
class SvgViewer : public QGLWidget
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
	SvgViewer(QWidget *parent);
	~SvgViewer();

	const Camera& camera()const{ return m_camera; }
	Camera& camera(){ return m_camera; }
	void resetCamera();

	svg::SvgManager* getSvgManager();

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void timerEvent(QTimerEvent *) { update(); }

	void toggleShowType(int t){ if (m_showType & t) m_showType -= t; else m_showType |= t; }
	int getShowType()const{ return m_showType; }
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*); 
	void renderFbo();
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

	svg::SvgManager* m_svgManager;
	int m_svgLastHighLightId;
};

#endif // SvgViewer_H
