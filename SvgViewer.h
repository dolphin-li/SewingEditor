#ifndef SvgViewer_H
#define SvgViewer_H

#include <QtOpenGL>
#include "camera.h"
#include "event_handles\AbstractEventHandle.h"
namespace svg{
	class SvgManager;
}
class SvgViewer : public QGLWidget
{
	Q_OBJECT

public:
	SvgViewer(QWidget *parent);
	~SvgViewer();
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	const Camera& camera()const{ return m_camera; }
	Camera& camera(){ return m_camera; }
	void resetCamera();
	svg::SvgManager* getSvgManager();
	void setSvgManager(svg::SvgManager* manager);
	Qt::MouseButtons buttons()const{ return m_buttons; }
	QPoint lastMousePos()const{ return m_lastPos; }
	const QImage& fboImage()const{ return m_fboImage; }
	AbstractEventHandle::ProcessorType getEventHandleType()const;
	void setEventHandleType(AbstractEventHandle::ProcessorType type);
	const AbstractEventHandle* getEventHandle(AbstractEventHandle::ProcessorType type)const;
	 AbstractEventHandle* getEventHandle(AbstractEventHandle::ProcessorType type);

	void beginDragBox(QPoint p);
	void endDragBox();

protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*); 
	void renderFbo();
	void renderDragBox();
protected:
	Camera m_camera;
	QPoint m_lastPos;
	Qt::MouseButtons m_buttons;
	QGLFramebufferObject* m_fbo;
	QImage m_fboImage;
	svg::SvgManager* m_svgManager;

	bool m_isDragBox;
	QPoint m_dragBoxBegin;
protected:
	AbstractEventHandle* m_currentEventHandle;
	std::vector<std::shared_ptr<AbstractEventHandle>> m_eventHandles;
};

#endif // SvgViewer_H
