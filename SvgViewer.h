#ifndef SvgViewer_H
#define SvgViewer_H

#include <QtOpenGL>
#include "Camera\camera.h"
#include "event_handles\AbstractEventHandle.h"
namespace svg{
	class SvgManager;
}
class FreeFormDeform;
class SvgViewer : public QGLWidget
{
	Q_OBJECT

public:
	SvgViewer(QWidget *parent);
	~SvgViewer();
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	const ldp::Camera& camera()const{ return m_camera; }
	ldp::Camera& camera(){ return m_camera; }
	void resetCamera();
	svg::SvgManager* getSvgManager();
	void setSvgManager(svg::SvgManager* manager);
	FreeFormDeform* getSvgDeformer();
	void setSvgDeformer(FreeFormDeform* deformer);
	Qt::MouseButtons buttons()const{ return m_buttons; }
	QPoint lastMousePos()const{ return m_lastPos; }
	const QImage& fboImage()const{ return m_fboImage; }
	AbstractEventHandle::ProcessorType getEventHandleType()const;
	void setEventHandleType(AbstractEventHandle::ProcessorType type);
	const AbstractEventHandle* getEventHandle(AbstractEventHandle::ProcessorType type)const;
	 AbstractEventHandle* getEventHandle(AbstractEventHandle::ProcessorType type);

	void beginDragBox(QPoint p);
	void endDragBox();

	void beginDeformMode();
	void endDeformMode();
	int getActiveDeformNodeId()const { return m_activeDeformNodeId; }
	void setActiveDeformNodeId(int i) { m_activeDeformNodeId = i; }
	int getHighLightedDeformNodeId()const { return m_highLightedDeformNodeId; }
	void setHighLightedDeformNodeId(int i) { m_highLightedDeformNodeId = i; }

	ldp::Float2 svgCoord2viewCoord(ldp::Float2 p)const;
	ldp::Float2 viewCoord2svgCoord(ldp::Float2 p)const;
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
	void renderDeformMode(bool idxMode);
protected:
	ldp::Camera m_camera;
	QPoint m_lastPos;
	Qt::MouseButtons m_buttons;
	QGLFramebufferObject* m_fbo;
	QImage m_fboImage;
	svg::SvgManager* m_svgManager;
	FreeFormDeform* m_svgDeformer;

	bool m_isDragBox;
	QPoint m_dragBoxBegin;
	bool m_isDeformMode;
	int m_activeDeformNodeId;
	int m_highLightedDeformNodeId;
protected:
	AbstractEventHandle* m_currentEventHandle;
	std::vector<std::shared_ptr<AbstractEventHandle>> m_eventHandles;
};

#endif // SvgViewer_H
