#include <GL\glew.h>
#include "basemeshviewer.h"
#include "glut.h"
#include "global_data_holder.h"
#include "Renderable.h"

BaseMeshViewer::BaseMeshViewer(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	setMouseTracking(true);
	m_buttons = Qt::MouseButton::NoButton;
	m_isDragBox = false;

	m_eventHandles.resize((size_t)AbstractMeshEventHandle::ProcessorTypeEnd, nullptr);
	for (size_t i = (size_t)AbstractMeshEventHandle::ProcessorTypeGeneral;
		i < (size_t)AbstractMeshEventHandle::ProcessorTypeEnd; i++)
	{
		m_eventHandles[i] = std::shared_ptr<AbstractMeshEventHandle>(
			AbstractMeshEventHandle::create(AbstractMeshEventHandle::ProcessorType(i), this));
	}
	setEventHandleType(AbstractMeshEventHandle::ProcessorTypeGeneral);
}

BaseMeshViewer::~BaseMeshViewer()
{

}

void BaseMeshViewer::resetCamera()
{
	m_camera.setPerspective(60, float(width()) / float(height()), 0.1, 100);
	m_camera.setScalar(1);
	m_camera.lookAt(ldp::Float3(0, 0, 0), ldp::Float3(0, 0, -1), ldp::Float3(0, 1, 0));
}

void BaseMeshViewer::initializeGL()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_FRONT_AND_BACK);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	float diffuse[3] = { 0, 0, 0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, diffuse);

	m_showType = Renderable::SW_E | Renderable::SW_F | Renderable::SW_V | Renderable::SW_LIGHTING
		| Renderable::SW_FLAT | Renderable::SW_TEXTURE;

	resetCamera();

	m_shaderManager.create("./Shader");

	// fbo
	QGLFramebufferObjectFormat fmt;
	fmt.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
	fmt.setMipmap(true);
	m_fbo = new QGLFramebufferObject(width(), height(), fmt);
	if (!m_fbo->isValid())
		printf("error: invalid depth fbo!\n");
	if (glGetError() != GL_NO_ERROR)
		printf("%s\n", gluErrorString(glGetError()));
}

void BaseMeshViewer::resizeGL(int w, int h)
{
	m_camera.setViewPort(0, w, 0, h);
	m_camera.setPerspective(m_camera.getFov(), float(w) / float(h), 
		m_camera.getFrustumNear(), m_camera.getFrustumFar());

	if (m_fbo)
		delete m_fbo;
	QGLFramebufferObjectFormat fmt;
	fmt.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
	fmt.setMipmap(true);
	m_fbo = new QGLFramebufferObject(width(), height(), fmt);
}

void BaseMeshViewer::paintGL()
{
	// we first render for selection
	renderSelectionOnFbo();

	// then we do formal rendering=========================
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void BaseMeshViewer::renderSelectionOnFbo()
{

}

void BaseMeshViewer::mousePressEvent(QMouseEvent *ev)
{
	setFocus();
	m_lastPos = ev->pos();
	m_buttons = ev->buttons();

	m_currentEventHandle->mousePressEvent(ev);

	updateGL();
}

void BaseMeshViewer::keyPressEvent(QKeyEvent*ev)
{
	m_currentEventHandle->keyPressEvent(ev);
}

void BaseMeshViewer::keyReleaseEvent(QKeyEvent*ev)
{
	m_currentEventHandle->keyReleaseEvent(ev);
}

void BaseMeshViewer::mouseReleaseEvent(QMouseEvent *ev)
{
	m_currentEventHandle->mouseReleaseEvent(ev);

	// clear buttons
	m_buttons = Qt::NoButton;
	updateGL();
}

void BaseMeshViewer::mouseMoveEvent(QMouseEvent*ev)
{
	m_currentEventHandle->mouseMoveEvent(ev);

	// backup last position
	m_lastPos = ev->pos();
	updateGL();
}

void BaseMeshViewer::wheelEvent(QWheelEvent*ev)
{
	m_currentEventHandle->wheelEvent(ev);

	updateGL();
}

AbstractMeshEventHandle::ProcessorType BaseMeshViewer::getEventHandleType()const
{
	return m_currentEventHandle->type();
}

void BaseMeshViewer::setEventHandleType(AbstractMeshEventHandle::ProcessorType type)
{
	m_currentEventHandle = m_eventHandles[size_t(type)].get();
	setCursor(m_currentEventHandle->cursor());
}

const AbstractMeshEventHandle* BaseMeshViewer::getEventHandle(AbstractMeshEventHandle::ProcessorType type)const
{
	return m_eventHandles[size_t(type)].get();
}

AbstractMeshEventHandle* BaseMeshViewer::getEventHandle(AbstractMeshEventHandle::ProcessorType type)
{
	return m_eventHandles[size_t(type)].get();
}

void BaseMeshViewer::beginDragBox(QPoint p)
{
	m_dragBoxBegin = p;
	m_isDragBox = true;
}

void BaseMeshViewer::endDragBox()
{
	m_isDragBox = false;
}

void BaseMeshViewer::renderDragBox()
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


