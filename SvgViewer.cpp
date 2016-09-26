#include "glew.h"
#include "SvgViewer.h"
#include "glut.h"
#include "global_data_holder.h"
#include "Renderable.h"
#include "svgpp\SvgManager.h"

SvgViewer::SvgViewer(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	setMouseTracking(true);
	m_buttons = Qt::MouseButton::NoButton;
	m_meshOperationMode = ObjectMode;
	m_isBoxMode = false;
	m_svgManager= new svg::SvgManager();
}

SvgViewer::~SvgViewer()
{
	delete m_svgManager;
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
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
	glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

	resetCamera();

	// depth fbo
	QGLFramebufferObjectFormat fmt;
	fmt.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
	fmt.setMipmap(true);
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
	fmt.setMipmap(true);
	m_fbo = new QGLFramebufferObject(width(), height(), fmt);
}

svg::SvgManager* SvgViewer::getSvgManager()
{
	return m_svgManager;
}

void SvgViewer::setMeshOpMode(MeshOperationMode mode)
{ 
	m_meshOperationMode = mode;
}

void SvgViewer::paintGL()
{
	glClearStencil(0);
	glStencilMask(~0);
	glClearColor(1,1,1, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_camera.apply();

	m_svgManager->render();
}

void SvgViewer::keyPressEvent(QKeyEvent*ev)
{
	bool noMod = ((ev->modifiers() & Qt::SHIFT) == 0)
		&& ((ev->modifiers() & Qt::CTRL) == 0)
		& ((ev->modifiers() & Qt::ALT) == 0);
	switch (ev->key())
	{
	case Qt::Key_Shift:
		break;
	case Qt::Key_B:
		m_isBoxMode = true;
		m_boxBegin = m_boxEnd = m_lastPos;
		break;
	case Qt::Key_Escape:
		m_isBoxMode = false;
		break;
	case Qt::Key_F:
		if (noMod)
		{
			toggleShowType(Renderable::SW_F);
			//toggleShowType(Renderable::SW_E);
		}
		break;
	case Qt::Key_L:
		if (noMod)
		{
			//toggleShowType(Renderable::SW_F);
			toggleShowType(Renderable::SW_E);
		}
		break;
	case Qt::Key_V:
		if (noMod)
			toggleShowType(Renderable::SW_V);
		break;
	case Qt::Key_T:
		if (noMod)
			toggleShowType(Renderable::SW_TEXTURE);
		break;
	case Qt::Key_S:
		if (1)
		{
			QString name = QFileDialog::getSaveFileName(this, "grab gl buffer", "data_test", "*.png");
			if (!name.isEmpty())
			{
				if (!name.toLower().endsWith(".png"))
					name.append(".png");
				QImage img = grabFrameBuffer();
				img.save(name);
			}
		}
		break;
	case Qt::Key_Space:
		toggleMeshOperationMode();
		break;
	default:
		break;
	}
	updateGL();
}

void SvgViewer::keyReleaseEvent(QKeyEvent*ev)
{

}


void SvgViewer::mousePressEvent(QMouseEvent *ev)
{
	setFocus();
	m_lastPos = ev->pos();
	m_buttons = ev->buttons();

	// move operation
	if (ev->button() == Qt::MouseButton::LeftButton)
	{

	}
}


void SvgViewer::mouseDoubleClickEvent(QMouseEvent *ev)
{
	setFocus();
	m_lastPos = ev->pos();
	m_buttons = ev->buttons();

	// move operation
	if (ev->button() == Qt::MouseButton::LeftButton)
	{

	}
	if (ev->button() == Qt::MouseButton::MiddleButton)
	{
		resetCamera();
		updateGL();
	}
}

void SvgViewer::mouseReleaseEvent(QMouseEvent *ev)
{
	// clear buttons
	m_isBoxMode = false;
	m_buttons = Qt::NoButton;
	updateGL();
}

void SvgViewer::mouseMoveEvent(QMouseEvent*ev)
{
	if (m_buttons == Qt::NoButton)
	{
		if (m_isBoxMode)
		{
			m_boxEnd = ev->pos();
			ldp::Float2 bMin(m_boxBegin.x(), height() - 1 - m_boxBegin.y());
			ldp::Float2 bMax(m_boxEnd.x(), height() - 1 - m_boxEnd.y());
			if (bMin[0] > bMax[0]) std::swap(bMin[0], bMax[0]);
			if (bMin[1] > bMax[1]) std::swap(bMin[1], bMax[1]);
		}
	}

	if (m_buttons & Qt::LeftButton)
	{

	}

	if (m_buttons & Qt::RightButton)
	{

	}

	if (m_buttons & Qt::MidButton)
	{
		float l = m_camera.getFrustumLeft();
		float r = m_camera.getFrustumRight();
		float t = m_camera.getFrustumTop();
		float b = m_camera.getFrustumBottom();
		float dx = (r - l) / float(width()) * (ev->pos().x() - m_lastPos.x());
		float dy = (b - t) / float(height()) * (ev->pos().y() - m_lastPos.y());
		l -= dx;
		r -= dx;
		t -= dy;
		b -= dy;
		m_camera.setFrustum(l, r, t, b, m_camera.getFrustumNear(), m_camera.getFrustumFar());
	}

	// backup last position
	m_lastPos = ev->pos();
	updateGL();
}

void SvgViewer::wheelEvent(QWheelEvent*ev)
{
	float s = 1.2f;
	if (ev->delta() < 0)
		s = 1.f / s;

	float l = m_camera.getFrustumLeft();
	float r = m_camera.getFrustumRight();
	float t = m_camera.getFrustumTop();
	float b = m_camera.getFrustumBottom();
	float dx = float(ev->pos().x()) / float(width()) * (r - l);
	float dy = float(ev->pos().y()) / float(height()) * (b - t);

	r = dx + l + (r - l - dx) * s;
	l = dx + l - dx * s;
	b = dy + t + (b - t - dy) * s;
	t = dy + t - dy * s;
	m_camera.setFrustum(l, r, t, b, m_camera.getFrustumNear(), m_camera.getFrustumFar());
	
	updateGL();
}

void SvgViewer::toggleMeshOperationMode()
{
	if (m_meshOperationMode == ObjectMode)
	{
		m_meshOperationMode = EditMode;
		printf("EditMode\n");
	}
	else
	{
		m_meshOperationMode = ObjectMode;
		printf("ObjectMode\n");
	}
}


