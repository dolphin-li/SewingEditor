#include "glew.h"
#include "SvgViewer.h"
#include "glut.h"
#include "global_data_holder.h"
#include "Renderable.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgAbstractObject.h"

SvgViewer::SvgViewer(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	setMouseTracking(true);
	m_buttons = Qt::MouseButton::NoButton;
	m_svgManager.reset(new svg::SvgManager());
	m_svgShapeToRender = svg::SvgAbstractObject::ShapeAll;
	m_isDragBox = false;

	m_eventHandles.resize((size_t)AbstractEventHandle::ProcessorTypeEnd, nullptr);
	for (size_t i = (size_t)AbstractEventHandle::ProcessorTypeGeneral; 
		i < (size_t)AbstractEventHandle::ProcessorTypeEnd; i++)
	{
		m_eventHandles[i] = std::shared_ptr<AbstractEventHandle>(
			AbstractEventHandle::create(AbstractEventHandle::ProcessorType(i), this));
	}
	setEventHandleType(AbstractEventHandle::ProcessorTypeShape);
}

SvgViewer::~SvgViewer()
{

}

AbstractEventHandle::ProcessorType SvgViewer::getEventHandleType()const
{
	return m_currentEventHandle->type();
}

void SvgViewer::setEventHandleType(AbstractEventHandle::ProcessorType type)
{
	m_currentEventHandle = m_eventHandles[size_t(type)].get();
	setCursor(m_currentEventHandle->cursor());
}

const AbstractEventHandle* SvgViewer::getEventHandle(AbstractEventHandle::ProcessorType type)const
{
	return m_eventHandles[size_t(type)].get();
}

AbstractEventHandle* SvgViewer::getEventHandle(AbstractEventHandle::ProcessorType type)
{
	return m_eventHandles[size_t(type)].get();
}

void SvgViewer::resetCamera()
{
	m_camera.setViewPort(0, width(), 0, height());
	m_camera.enableOrtho(true);
	m_camera.setFrustum(0, width(), 0, height(), -1, 1);
	m_camera.lookAt(ldp::Float3(0, 0, 0), ldp::Float3(0, 0, -1), ldp::Float3(0, 1, 0));
	if (m_svgManager)
		m_camera.setFrustum(0, m_svgManager->width(), 0, m_svgManager->height(), -1, 1);
}

void SvgViewer::initializeGL()
{
	glewInit();

	resetCamera();

	// depth fbo
	QGLFramebufferObjectFormat fmt;
	fmt.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
	m_fbo = new QGLFramebufferObject(width(), height(), fmt);
	if (!m_fbo->isValid())
		printf("error: invalid depth fbo!\n");
	if (glGetError() != GL_NO_ERROR)
		printf("%s\n", gluErrorString(glGetError()));

	m_svgManager->init(&m_camera);
}

void SvgViewer::resizeGL(int w, int h)
{
	m_camera.setViewPort(0, w, 0, h);

	if (m_fbo)
		delete m_fbo;
	QGLFramebufferObjectFormat fmt;
	fmt.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
	m_fbo = new QGLFramebufferObject(w, h, fmt);
}

svg::SvgManager* SvgViewer::getSvgManager()
{
	return m_svgManager.get();
}

void SvgViewer::setSvgManager(std::shared_ptr<svg::SvgManager> manager)
{
	m_svgManager = manager;
}

void SvgViewer::paintGL()
{
	renderFbo();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
	glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
	glClearStencil(0);
	glStencilMask(~0);
	glClearColor( 1, 1, 1, 0);
	glColor4f(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	m_camera.apply();
	renderDragBox();
	m_svgManager->render(m_svgShapeToRender);
}

void SvgViewer::renderFbo()
{
	m_fbo->bind();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NEAREST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
	glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
	glClearStencil(0);
	glStencilMask(~0);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_camera.apply();
	m_svgManager->renderIndex(m_svgShapeToRender);

	m_fbo->release();
	m_fboImage = m_fbo->toImage();
}

void SvgViewer::beginDragBox(QPoint p)
{
	m_dragBoxBegin = p;
	m_isDragBox = true;
}

void SvgViewer::endDragBox()
{
	m_isDragBox = false;
}

void SvgViewer::renderDragBox()
{
	if (!m_isDragBox)
		return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	float l = camera().getFrustumLeft();
	float r = camera().getFrustumRight();
	float t = camera().getFrustumTop();
	float b = camera().getFrustumBottom();
	float x0 = std::min(m_dragBoxBegin.x(), m_lastPos.x()) / float(width()) * (r - l) + l;
	float x1 = std::max(m_dragBoxBegin.x(), m_lastPos.x()) / float(width()) * (r - l) + l;
	float y0 = std::min(m_dragBoxBegin.y(), m_lastPos.y()) / float(height()) * (b - t) + t;
	float y1 = std::max(m_dragBoxBegin.y(), m_lastPos.y()) / float(height()) * (b - t) + t;

	glDisable(GL_STENCIL_TEST);
	glColor3f(0, 1, 0);
	glLineWidth(2);
	//glEnable(GL_LINE_STIPPLE);
	glLineStipple(0xAAAA, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x0, y0);
	glVertex2f(x0, y1);
	glVertex2f(x1, y1);
	glVertex2f(x1, y0);
	glEnd();

	glPopAttrib();
}

void SvgViewer::keyPressEvent(QKeyEvent*ev)
{
	m_currentEventHandle->keyPressEvent(ev);
}

void SvgViewer::keyReleaseEvent(QKeyEvent*ev)
{
	m_currentEventHandle->keyReleaseEvent(ev);
}

void SvgViewer::mousePressEvent(QMouseEvent *ev)
{
	setFocus();
	m_lastPos = ev->pos();
	m_buttons = ev->buttons();

	m_currentEventHandle->mousePressEvent(ev);

	updateGL();
}

void SvgViewer::mouseDoubleClickEvent(QMouseEvent *ev)
{
	setFocus();
	m_lastPos = ev->pos();
	m_buttons = ev->buttons();

	m_currentEventHandle->mouseDoubleClickEvent(ev);

	updateGL();
}

void SvgViewer::mouseReleaseEvent(QMouseEvent *ev)
{
	m_currentEventHandle->mouseReleaseEvent(ev);

	// clear buttons
	m_buttons = Qt::NoButton;
	updateGL();
}

void SvgViewer::mouseMoveEvent(QMouseEvent*ev)
{
	m_currentEventHandle->mouseMoveEvent(ev);

	// backup last position
	m_lastPos = ev->pos();
	updateGL();
}

void SvgViewer::wheelEvent(QWheelEvent*ev)
{
	m_currentEventHandle->wheelEvent(ev);

	updateGL();
}

void SvgViewer::loadSvg(QString name)
{
	m_svgManager->load(name.toStdString().c_str());
}



