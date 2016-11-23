#include <GL\glew.h>
#include <QtOpenGL>
#include <GL\glut.h>
#include "SimViewer.h"
#include "adaptiveCloth\simulation.hpp"
#include "ldpMat\Quaternion.h"
#include "SimulationManager.h"


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

SimViewer::SimViewer(QWidget *parent)
{
	setMouseTracking(true);
	m_buttons = Qt::MouseButton::NoButton;
	m_isDragBox = false;
	m_isEdgeMode = false;
	m_trackBallMode = TrackBall_None;
	m_currentEventHandle = nullptr;
	m_fbo = nullptr;

	m_eventHandles.resize((size_t)AbstractSimEventHandle::ProcessorTypeEnd, nullptr);
	for (size_t i = (size_t)AbstractSimEventHandle::ProcessorTypeGeneral;
		i < (size_t)AbstractSimEventHandle::ProcessorTypeEnd; i++)
	{
		m_eventHandles[i] = std::shared_ptr<AbstractSimEventHandle>(
			AbstractSimEventHandle::create(AbstractSimEventHandle::ProcessorType(i), this));
	}
	setEventHandleType(AbstractSimEventHandle::ProcessorTypeGeneral);
	m_simManager = nullptr;
	m_fps = 0;
	m_computeTimer = startTimer(1000);
	m_renderTimer = startTimer(30);
}

SimViewer::~SimViewer()
{

}

void SimViewer::resetCamera()
{
	m_camera.setPerspective(60, float(width()) / float(height()), 0.1, 100);
	ldp::Float3 c = 0.f;
	float l = 1.f;
	if (m_simManager)
	if (m_simManager->getSimulator())
	{
		ldp::Float3 bmin, bmax;
		getModelBound(bmin, bmax);
		c = (bmax + bmin) / 2.f;
		l = (bmax - bmin).length();
	}
	m_camera.lookAt(ldp::Float3(0, -l, 0) + c, c, ldp::Float3(0, 0, 1));
	m_camera.arcballSetCenter(c);
}

void SimViewer::initializeGL()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_FRONT_AND_BACK);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1, 1);

	resetCamera();

	// fbo
	QGLFramebufferObjectFormat fmt;
	fmt.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
	m_fbo = new QGLFramebufferObject(width(), height(), fmt);
	if (!m_fbo->isValid())
		printf("error: invalid depth fbo!\n");
}

void SimViewer::resizeGL(int w, int h)
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

void SimViewer::timerEvent(QTimerEvent* ev)
{
	if (!m_simManager)
		return;
	if (ev->timerId() == m_computeTimer && m_simManager)
	{
		m_simManager->simulate(1);
	}
	if (ev->timerId() == m_renderTimer)
		updateGL();
}

void SimViewer::paintGL()
{
	// we first render for selection
	renderSelectionOnFbo();

	// then we do formal rendering=========================
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// show cloth simulation=============================
	m_camera.apply();
	renderCloth(0);
	renderBody(0);
	renderTrackBall(false);
	renderDragBox();
}

void SimViewer::renderSelectionOnFbo()
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

void SimViewer::mousePressEvent(QMouseEvent *ev)
{
	setFocus();
	m_lastPos = ev->pos();
	m_buttons = ev->buttons();

	m_currentEventHandle->mousePressEvent(ev);

	updateGL();
}

void SimViewer::keyPressEvent(QKeyEvent*ev)
{
	m_currentEventHandle->keyPressEvent(ev);
	updateGL();
}

void SimViewer::keyReleaseEvent(QKeyEvent*ev)
{
	m_currentEventHandle->keyReleaseEvent(ev);
	updateGL();
}

void SimViewer::mouseReleaseEvent(QMouseEvent *ev)
{
	m_currentEventHandle->mouseReleaseEvent(ev);

	// clear buttons
	m_buttons = Qt::NoButton;
	updateGL();
}

void SimViewer::mouseMoveEvent(QMouseEvent*ev)
{
	m_currentEventHandle->mouseMoveEvent(ev);

	// backup last position
	m_lastPos = ev->pos();
	updateGL();
}

void SimViewer::mouseDoubleClickEvent(QMouseEvent *ev)
{
	m_currentEventHandle->mouseDoubleClickEvent(ev);

	updateGL();
}

void SimViewer::wheelEvent(QWheelEvent*ev)
{
	m_currentEventHandle->wheelEvent(ev);

	updateGL();
}

AbstractSimEventHandle::ProcessorType SimViewer::getEventHandleType()const
{
	return m_currentEventHandle->type();
}

void SimViewer::setEventHandleType(AbstractSimEventHandle::ProcessorType type)
{
	if (m_currentEventHandle)
		m_currentEventHandle->handleLeave();
	m_currentEventHandle = m_eventHandles[size_t(type)].get();
	m_currentEventHandle->handleEnter();
	setCursor(m_currentEventHandle->cursor());
}

const AbstractSimEventHandle* SimViewer::getEventHandle(AbstractSimEventHandle::ProcessorType type)const
{
	return m_eventHandles[size_t(type)].get();
}

AbstractSimEventHandle* SimViewer::getEventHandle(AbstractSimEventHandle::ProcessorType type)
{
	return m_eventHandles[size_t(type)].get();
}

void SimViewer::beginDragBox(QPoint p)
{
	m_dragBoxBegin = p;
	m_isDragBox = true;
}

void SimViewer::endDragBox()
{
	m_isDragBox = false;
}

void SimViewer::renderDragBox()
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

void SimViewer::initCloth(arcsim::SimulationManager* sim)
{
	if (sim == nullptr)
		throw std::exception("initCloth(): nullptr error");
	m_simManager = sim;
}

void SimViewer::getModelBound(ldp::Float3& bmin, ldp::Float3& bmax)
{
	bmin = FLT_MAX;
	bmax = -FLT_MAX;
	if (m_simManager == nullptr)
		return;
	
	const auto& sim = m_simManager->getSimulator();
	if (sim == nullptr)
		return;
	for (const auto& obs : sim->obstacles)
	{
		if (!obs.activated)
			continue;
		const auto& mesh = obs.base_mesh;
		for (const auto& node : mesh.nodes)
		{
			for (int k = 0; k < 3; k++)
			{
				bmin[k] = std::min(bmin[k], (float)node->x[k]);
				bmax[k] = std::max(bmax[k], (float)node->x[k]);
			}	
		} // end for nodes
	} // end for obs
	
}

void SimViewer::beginTrackBall(TrackBallMode mode, ldp::Float3 p, ldp::Mat3f R, float scale)
{
	m_trackBallPos = p;
	m_trackBallR = R;
	m_trackBallScale = scale;
	m_trackBallMode = mode;
	m_activeTrackBallAxis = -1;
	m_hoverTrackBallAxis = -1;
}

void SimViewer::rotateTrackBall(ldp::Mat3d R)
{
	m_trackBallR = R * m_trackBallR;
}

void SimViewer::translateTrackBall(ldp::Double3 t)
{
	m_trackBallPos += t;
}

void SimViewer::endTrackBall()
{
	m_trackBallMode = TrackBall_None;
	m_activeTrackBallAxis = -1;
	m_hoverTrackBallAxis = -1;
}

int SimViewer::fboRenderedIndex(QPoint p)const
{
	if (m_fboImage.rect().contains(p))
	{
		QRgb c = m_fboImage.pixel(p);
		return colorToSelectId(ldp::Float4(qRed(c), qGreen(c), qBlue(c), qAlpha(c))/255.f);
	}
	return 0;
}

void SimViewer::renderTrackBall(bool indexMode)
{
	if (m_trackBallMode == TrackBall_None)
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
	if (m_trackBallMode == TrackBall_Rot)
		glutSolidTorus(m_trackBallScale * 0.03, m_trackBallScale, 16, 128);
	else if (m_trackBallMode == TrackBall_Trans)
		solid_axis(m_trackBallScale * 0.03, m_trackBallScale);
	glMultMatrixf(get_z2x_rot().trans().ptr());

	// y axis
	glColor3f(0, 1, 0);
	if (m_activeTrackBallAxis == TrackBallIndex_Y || (m_hoverTrackBallAxis == 
		TrackBallIndex_Y && m_activeTrackBallAxis < 0))
		glColor3f(1, 1, 1);
	if (indexMode)
		glColor4fv(selectIdToColor(TrackBallIndex_Y).ptr());
	glMultMatrixf(get_z2y_rot().ptr());
	if (m_trackBallMode == TrackBall_Rot)
		glutSolidTorus(m_trackBallScale * 0.03, m_trackBallScale, 16, 128);
	else if (m_trackBallMode == TrackBall_Trans)
		solid_axis(m_trackBallScale * 0.03, m_trackBallScale);
	glMultMatrixf(get_z2y_rot().trans().ptr());

	// z axis
	glColor3f(0, 0, 1);
	if (m_activeTrackBallAxis == TrackBallIndex_Z || (m_hoverTrackBallAxis == 
		TrackBallIndex_Z && m_activeTrackBallAxis < 0))
		glColor3f(1, 1, 1);
	if (indexMode)
		glColor4fv(selectIdToColor(TrackBallIndex_Z).ptr());
	if (m_trackBallMode == TrackBall_Rot)
		glutSolidTorus(m_trackBallScale * 0.03, m_trackBallScale, 16, 128);
	else if (m_trackBallMode == TrackBall_Trans)
		solid_axis(m_trackBallScale * 0.03, m_trackBallScale);

	// sphere
	if (!indexMode && m_trackBallMode == TrackBall_Rot)
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

void SimViewer::renderCloth(int idxMode)
{
	if (!m_simManager)
		return;
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	const auto& sim = m_simManager->getSimulator();

	if (idxMode == 0)
	{
		if (m_isEdgeMode)
		{
			glDisable(GL_LIGHTING);
			glColor3f(0, 1, 0);
			glBegin(GL_LINES);
			for (const auto& cloth : sim->cloths)
			{
				const auto& mesh = cloth.mesh;
				for (const auto& face : mesh.faces)
				{
					for (int k = 0; k < 3; k++)
					{
						glVertex3dv(&face->v[k]->node->x[0]);
						glVertex3dv(&face->v[(k+1)%3]->node->x[0]);
					}
				} // end for face
			} // end for obs
			glEnd();
		}
		else
		{
			float diff[3] = { 0.8, 0.8, 0.8 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
			glBegin(GL_TRIANGLES);
			for (const auto& cloth : sim->cloths)
			{
				const auto& mesh = cloth.mesh;
				for (const auto& face : mesh.faces)
				{
					for (int k = 0; k < 3; k++)
					{
						glNormal3dv(&face->v[k]->node->n[0]);
						glVertex3dv(&face->v[k]->node->x[0]);
					}
				} // end for face
			} // end for obs
			glEnd();
		}// end if not edge mode
	} // end if idxMode == 0

	glPopAttrib();
}

void SimViewer::renderBody(int idxMode)
{
	if (!m_simManager)
		return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	const auto& sim = m_simManager->getSimulator();

	if (idxMode == 0)
	{
		float diff[3] = {0.5, 0.6, 0.8};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
		glBegin(GL_TRIANGLES);
		for (const auto& obs : sim->obstacles)
		{
			if (!obs.activated)
				continue;
			const auto& mesh = obs.curr_state_mesh;
			for (const auto& face : mesh.faces)
			{
				for (int k = 0; k < 3; k++)
				{
					glNormal3dv(&face->v[k]->node->n[0]);
					glVertex3dv(&face->v[k]->node->x[0]);
				}
			} // end for face
		} // end for obs
		glEnd();
	} // end if idxMode == 0

	glPopAttrib();
}