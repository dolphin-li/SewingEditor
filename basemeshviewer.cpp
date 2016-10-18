#include <GL\glew.h>
#include "basemeshviewer.h"
#include "designer2d_cloth.h"
#include "analysis2d_cloth_static.h"
#include "ldpMat\Quaternion.h"
#pragma region --mat_utils

inline ldp::Mat3f angles2rot(ldp::Float3 v)
{
	float theta = v.length();
	if (theta == 0)
		return ldp::Mat3f().eye();
	v /= theta;
	return ldp::QuaternionF().fromAngleAxis(theta, v).toRotationMatrix3();
}

inline ldp::Float3 rot2angles(ldp::Mat3f R)
{
	ldp::QuaternionF q;
	q.fromRotationMatrix(R);
	ldp::Float3 v;
	float ag;
	q.toAngleAxis(v, ag);
	v *= ag;
	return v;
}

static GLUquadric* get_quadric()
{
	static GLUquadric* q = gluNewQuadric();
	return q;
}

static ldp::Mat4f get_z2x_rot()
{
	static ldp::Mat4f R = ldp::QuaternionF().fromRotationVecs(ldp::Float3(0, 0, 1),
		ldp::Float3(1, 0, 0)).toRotationMatrix();
	return R;
}

static ldp::Mat4f get_z2y_rot()
{
	static ldp::Mat4f R = ldp::QuaternionF().fromRotationVecs(ldp::Float3(0, 0, 1),
		ldp::Float3(0, 1, 0)).toRotationMatrix();
	return R;
}

static void solid_axis(float base, float length)
{
	GLUquadric* q = get_quadric();
	gluCylinder(q, base, base, length, 32, 32);
	glTranslatef(0, 0, length);
	gluCylinder(q, base*2.5f, 0.f, length* 0.2f, 32, 32);
	glTranslatef(0, 0, -length);
}

inline int colorToSelectId(ldp::Float4 c)
{
	ldp::UInt4 cl = c*255.f;
	return (cl[0] << 24) + (cl[1] << 16) + (cl[2] << 8) + cl[3];
}

inline ldp::Float4 selectIdToColor(unsigned int id)
{
	int r = (id >> 24) & 0xff;
	int g = (id >> 16) & 0xff;
	int b = (id >> 8) & 0xff;
	int a = id & 0xff;
	return ldp::Float4(r, g, b, a) / 255.f;
}

#pragma endregion

BaseMeshViewer::BaseMeshViewer(QWidget *parent)
{
	setMouseTracking(true);
	m_buttons = Qt::MouseButton::NoButton;
	m_isDragBox = false;
	m_isEdgeMode = false;
	m_isTrackBall = false;

	m_eventHandles.resize((size_t)AbstractMeshEventHandle::ProcessorTypeEnd, nullptr);
	for (size_t i = (size_t)AbstractMeshEventHandle::ProcessorTypeGeneral;
		i < (size_t)AbstractMeshEventHandle::ProcessorTypeEnd; i++)
	{
		m_eventHandles[i] = std::shared_ptr<AbstractMeshEventHandle>(
			AbstractMeshEventHandle::create(AbstractMeshEventHandle::ProcessorType(i), this));
	}
	setEventHandleType(AbstractMeshEventHandle::ProcessorTypeClothSelect);


	m_pAnalysis = nullptr;
	m_pListener = nullptr;
	m_fps = 0;
	m_computeTimer = startTimer(1);
	m_renderTimer = startTimer(30);
}

BaseMeshViewer::~BaseMeshViewer()
{

}

void BaseMeshViewer::resetCamera()
{
	m_camera.setPerspective(60, float(width()) / float(height()), 0.1, 100);
	ldp::Float3 c = 0.f;
	float l = 1.f;
	if (m_pAnalysis)
	{
		ldp::Float3 bmin, bmax;
		getModelBound(bmin, bmax);
		c = (bmax + bmin) / 2.f;
		l = (bmax - bmin).length();
	}
	m_camera.lookAt(ldp::Float3(0, -l, 0) + c, c, ldp::Float3(0, 0, 1));
	m_camera.arcballSetCenter(c);
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
	//glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	resetCamera();

	m_shaderManager.create("./Shader");
	// fbo
	QGLFramebufferObjectFormat fmt;
	fmt.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
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

void BaseMeshViewer::timerEvent(QTimerEvent* ev)
{
	if (!m_pListener || !m_pAnalysis)
		return;
	if (ev->timerId() == m_computeTimer)
	{
		gtime_t t1 = ldp::gtime_now();
		m_pListener->FollowMshToCad_ifNeeded();
		m_pListener->Solve_ifNeeded();
		if (m_pAnalysis->IsBlowUp())
		{
			std::cout << "BlowUp" << std::endl;
			m_pListener->LoadTimeStamp();
		}
		gtime_t t2 = ldp::gtime_now();
		double sec = ldp::gtime_seconds(t1, t2);
		m_fps = 1 / sec;
	}
	if (ev->timerId() == m_renderTimer)
		updateGL();
}

void BaseMeshViewer::paintGL()
{
	// we first render for selection
	renderSelectionOnFbo();

	// then we do formal rendering=========================
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// show cloth simulation=============================
	m_camera.apply();
	if (m_pListener)
	{
		if (isEdgeMode())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_pListener->Draw(4);
	}
	renderTrackBall(false);
	renderDragBox();
}

void BaseMeshViewer::renderSelectionOnFbo()
{
	m_fbo->bind();
	glClearColor(0.f, 0.f, 0.f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	m_camera.apply();

	renderTrackBall(true);

	m_fboImage = m_fbo->toImage();
	m_fbo->release();

	glPopAttrib();
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
	updateGL();
}

void BaseMeshViewer::keyReleaseEvent(QKeyEvent*ev)
{
	m_currentEventHandle->keyReleaseEvent(ev);
	updateGL();
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

void BaseMeshViewer::mouseDoubleClickEvent(QMouseEvent *ev)
{
	m_currentEventHandle->mouseDoubleClickEvent(ev);

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

void BaseMeshViewer::initCloth(CAnalysis2D_Cloth_Static* pAnalysis, CDesigner2D_Cloth* pListener)
{
	if (pAnalysis == nullptr || pListener == nullptr)
		throw std::exception("initCloth(): nullptr error");
	m_pAnalysis = pAnalysis;
	m_pListener = pListener;
}

void BaseMeshViewer::getModelBound(ldp::Float3& bmin, ldp::Float3& bmax)
{
	bmin = FLT_MAX;
	bmax = -FLT_MAX;
	if (m_pAnalysis)
	{
		auto box = m_pAnalysis->GetBoundingBox(nullptr);
		bmin = ldp::Float3(box.x_min, box.y_min, box.z_min);
		bmax = ldp::Float3(box.x_max, box.y_max, box.z_max);
	}
}

void BaseMeshViewer::beginTrackBall(ldp::Float3 p, ldp::Mat3f R, float scale)
{
	m_trackBallPos = p;
	m_trackBallR = R;
	m_trackBallScale = scale;
	m_isTrackBall = true;
	m_activeTrackBallAxis = -1;
	m_hoverTrackBallAxis = -1;
}

void BaseMeshViewer::rotateTrackBall(ldp::Mat3d R)
{
	m_trackBallR = R * m_trackBallR;
}

void BaseMeshViewer::endTrackBall()
{
	m_isTrackBall = false;
	m_activeTrackBallAxis = -1;
	m_hoverTrackBallAxis = -1;
}

int BaseMeshViewer::fboRenderedIndex(QPoint p)const
{
	if (m_fboImage.rect().contains(p))
	{
		QRgb c = m_fboImage.pixel(p);
		return colorToSelectId(ldp::Float4(qRed(c), qGreen(c), qBlue(c), qAlpha(c))/255.f);
	}
	return 0;
}

void BaseMeshViewer::renderTrackBall(bool indexMode)
{
	if (!m_isTrackBall)
		return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if (!indexMode)
	{
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	glPushMatrix();
	glTranslatef(m_trackBallPos[0], m_trackBallPos[1], m_trackBallPos[2]);
	ldp::Mat4f M = ldp::Mat4f().eye();
	M.setRotationPart(m_trackBallR);
	glMultMatrixf(M.ptr());

	// x axis
	glColor3f(1, 0, 0);
	if (m_activeTrackBallAxis == TrackBallIndex_X || (m_hoverTrackBallAxis == 
		TrackBallIndex_X && m_activeTrackBallAxis < 0))
		glColor3f(1, 1, 1);
	if (indexMode)
		glColor4fv(selectIdToColor(TrackBallIndex_X).ptr());
	glMultMatrixf(get_z2x_rot().ptr());
	glutSolidTorus(m_trackBallScale * 0.03, m_trackBallScale, 16, 128);
	glMultMatrixf(get_z2x_rot().trans().ptr());

	// y axis
	glColor3f(0, 1, 0);
	if (m_activeTrackBallAxis == TrackBallIndex_Y || (m_hoverTrackBallAxis == 
		TrackBallIndex_Y && m_activeTrackBallAxis < 0))
		glColor3f(1, 1, 1);
	if (indexMode)
		glColor4fv(selectIdToColor(TrackBallIndex_Y).ptr());
	glMultMatrixf(get_z2y_rot().ptr());
	glutSolidTorus(m_trackBallScale * 0.03, m_trackBallScale, 16, 128);
	glMultMatrixf(get_z2y_rot().trans().ptr());

	// z axis
	glColor3f(0, 0, 1);
	if (m_activeTrackBallAxis == TrackBallIndex_Z || (m_hoverTrackBallAxis == 
		TrackBallIndex_Z && m_activeTrackBallAxis < 0))
		glColor3f(1, 1, 1);
	if (indexMode)
		glColor4fv(selectIdToColor(TrackBallIndex_Z).ptr());
	glutSolidTorus(m_trackBallScale * 0.03, m_trackBallScale, 4, 128);

	// sphere
	if (!indexMode)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.6, 0.6, 0.6, 0.5);
		glutSolidSphere(m_trackBallScale, 32, 32);
		glDisable(GL_BLEND);
	}

	glPopMatrix();
	glPopAttrib();
}