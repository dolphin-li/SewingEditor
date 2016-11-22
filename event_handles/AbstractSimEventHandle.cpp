#include <QEvent>
#include <GL\glew.h>
#include "SimViewer.h"
#include "AbstractSimEventHandle.h"

#include "global_data_holder.h"
#include "svgpp\SvgPolyPath.h"
#include "svgpp\SvgManager.h"

ldp::Mat3d AbstractSimEventHandle::rotation_from_uv(ldp::Double3 u, ldp::Double3 v)
{
	ldp::Double3 x = u.normalize();
	ldp::Double3 z = x.cross(v).normalize();
	ldp::Double3 y = z.cross(x);
	ldp::Mat3d R;
	R(0, 0) = x[0]; R(0, 1) = y[0]; R(0, 2) = z[0];
	R(1, 0) = x[1]; R(1, 1) = y[1]; R(1, 2) = z[1];
	R(2, 0) = x[2]; R(2, 1) = y[2]; R(2, 2) = z[2];
	return R;
}

AbstractSimEventHandle::AbstractSimEventHandle(SimViewer* v)
{
	m_viewer = v;
	m_lastHighlightShapeId = -1;
	m_currentSelectedId = -1;
	m_cursor = QCursor(Qt::CursorShape::ArrowCursor);
	m_iconFile = "";
	m_inactiveIconFile = "";
	m_toolTips = "general handle";
}

AbstractSimEventHandle::~AbstractSimEventHandle()
{

}

QString AbstractSimEventHandle::iconFile()const
{
	return m_iconFile;
}

QString AbstractSimEventHandle::inactiveIconFile()const
{
	return m_inactiveIconFile;
}

void AbstractSimEventHandle::handleEnter()
{
	m_viewer->setFocus();
}
void AbstractSimEventHandle::handleLeave()
{
	m_viewer->clearFocus();
}

QString AbstractSimEventHandle::toolTips()const
{
	return m_toolTips;
}

AbstractSimEventHandle* AbstractSimEventHandle::create(ProcessorType type, SimViewer* v)
{
	switch (type)
	{
	case AbstractSimEventHandle::ProcessorTypeGeneral:
		return new AbstractSimEventHandle(v);
	case AbstractSimEventHandle::ProcessorTypeEnd:
	default:
		return nullptr;
	}
}

void AbstractSimEventHandle::pickMesh(QPoint p, bool reset_if_none_picked)
{
	const ldp::Camera& cam = m_viewer->camera();
}

void AbstractSimEventHandle::mousePressEvent(QMouseEvent *ev)
{
	m_mouse_press_pt = ev->pos();

	// arcball drag
	if (ev->buttons() == Qt::LeftButton)
		m_viewer->camera().arcballClick(ldp::Float2(ev->x(), ev->y()));
}

void AbstractSimEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{

}

void AbstractSimEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::MouseButton::MiddleButton)
		m_viewer->resetCamera();
}

void AbstractSimEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	if (ev->buttons() == Qt::LeftButton)
		m_viewer->camera().arcballDrag(ldp::Float2(ev->x(), ev->y()));
	if (ev->buttons() == Qt::MidButton)
	{
		QPoint dif = ev->pos() - m_viewer->lastMousePos();
		ldp::Float3 bmin, bmax;
		m_viewer->getModelBound(bmin, bmax);
		float len = (bmax - bmin).length() / sqrt(3.f);
		ldp::Float3 t(-(float)dif.x() / m_viewer->width(), (float)dif.y() / m_viewer->height(), 0);
		m_viewer->camera().translate(t * len);
		m_viewer->camera().arcballSetCenter((bmin + bmax) / 2.f + t * len);
	}
}

void AbstractSimEventHandle::wheelEvent(QWheelEvent *ev)
{
	float s = 1.1;
	if (ev->delta() < 0)
		s = 1.f / s;

	float fov = std::max(1e-3f, std::min(160.f, m_viewer->camera().getFov()*s));
	m_viewer->camera().setPerspective(fov, m_viewer->camera().getAspect(),
		m_viewer->camera().getFrustumNear(), m_viewer->camera().getFrustumFar());
}

void AbstractSimEventHandle::keyPressEvent(QKeyEvent *ev)
{
	switch (ev->key())
	{
	default:
		break;
	case Qt::Key_E:
		if (ev->modifiers() == Qt::NoModifier)
			m_viewer->setEdgeMode(!m_viewer->isEdgeMode());
		break;
	}
}

void AbstractSimEventHandle::keyReleaseEvent(QKeyEvent *ev)
{

}
