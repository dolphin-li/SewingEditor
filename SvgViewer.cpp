#include "glew.h"
#include "SvgViewer.h"
#include "glut.h"
#include "global_data_holder.h"
#include "Renderable.h"
#include "svgpp\SvgRenderer.h"

SvgViewer::SvgViewer(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	setMouseTracking(true);
	m_buttons = Qt::MouseButton::NoButton;
	m_meshOperationMode = ObjectMode;
	m_isBoxMode = false;
	m_svgRenderer = new SvgRenderer();
}

SvgViewer::~SvgViewer()
{
	delete m_svgRenderer;
}

void SvgViewer::resetCamera()
{
	m_camera.setViewPort(0, width(), 0, height());
	m_camera.enableOrtho(true);
	m_camera.setFrustum(0, 500, 0, 400, -1, 1);
	m_camera.lookAt(ldp::Float3(0, 0, 0), ldp::Float3(0, 0, -1), ldp::Float3(0, 1, 0));
}

void SvgViewer::initializeGL()
{
	glewInit();
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


	// depth fbo
	QGLFramebufferObjectFormat fmt;
	fmt.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
	fmt.setMipmap(true);
	m_fbo = new QGLFramebufferObject(width(), height(), fmt);
	if (!m_fbo->isValid())
		printf("error: invalid depth fbo!\n");
	if (glGetError() != GL_NO_ERROR)
		printf("%s\n", gluErrorString(glGetError()));

	m_svgRenderer->init(&m_camera);
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

void SvgViewer::setMeshOpMode(MeshOperationMode mode)
{ 
	m_meshOperationMode = mode;
}

void SvgViewer::paintGL()
{
	glClearStencil(0);
	glStencilMask(~0);
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_camera.apply();

	m_svgRenderer->render();
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
	if (ev->button() == Qt::MouseButton::MiddleButton)
	{
		resetCamera();
		updateGL();
	}
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

	// backup last position
	m_lastPos = ev->pos();
	updateGL();
}

void SvgViewer::wheelEvent(QWheelEvent*ev)
{
	float s = 1.1;
	if (ev->delta() < 0)
		s = 1.f / s;

	float fov = std::max(1e-3f, std::min(90.f, m_camera.getFov()*s));
	m_camera.setPerspective(fov, m_camera.getAspect(), 
		m_camera.getFrustumNear(), m_camera.getFrustumFar());
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


