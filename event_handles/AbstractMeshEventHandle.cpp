#include <QEvent>
#include <GL\glew.h>
#include "BaseMeshViewer.h"

#include "AbstractMeshEventHandle.h"
#include "ClothMeshEventHandle.h"

AbstractMeshEventHandle::AbstractMeshEventHandle(BaseMeshViewer* v)
{
	m_viewer = v;
	m_lastHighlightShapeId = -1;
	m_currentSelectedId = -1;
	m_cursor = QCursor(Qt::CursorShape::ArrowCursor);
	m_iconFile = "";
	m_toolTips = "general handle";
}

AbstractMeshEventHandle::~AbstractMeshEventHandle()
{

}

QString AbstractMeshEventHandle::iconFile()const
{
	return m_iconFile;
}

QString AbstractMeshEventHandle::toolTips()const
{
	return m_toolTips;
}

AbstractMeshEventHandle* AbstractMeshEventHandle::create(ProcessorType type, BaseMeshViewer* v)
{
	switch (type)
	{
	case AbstractMeshEventHandle::ProcessorTypeGeneral:
		return new AbstractMeshEventHandle(v);
	case AbstractMeshEventHandle::ProcessorTypeCloth:
		return new ClothMeshEventHandle(v);
	case AbstractMeshEventHandle::ProcessorTypeEnd:
	default:
		return nullptr;
	}
}

void AbstractMeshEventHandle::mousePressEvent(QMouseEvent *ev)
{
	m_mouse_press_pt = ev->pos();

	if (ev->buttons() == Qt::LeftButton)
		m_viewer->camera().arcballClick(ldp::Float2(ev->x(), ev->y()));
}

void AbstractMeshEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{

}

void AbstractMeshEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::MouseButton::MiddleButton)
		m_viewer->resetCamera();
}

void AbstractMeshEventHandle::mouseMoveEvent(QMouseEvent *ev)
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

void AbstractMeshEventHandle::wheelEvent(QWheelEvent *ev)
{
	float s = 1.1;
	if (ev->delta() < 0)
		s = 1.f / s;

	float fov = std::max(1e-3f, std::min(160.f, m_viewer->camera().getFov()*s));
	m_viewer->camera().setPerspective(fov, m_viewer->camera().getAspect(),
		m_viewer->camera().getFrustumNear(), m_viewer->camera().getFrustumFar());
}

void AbstractMeshEventHandle::keyPressEvent(QKeyEvent *ev)
{

}

void AbstractMeshEventHandle::keyReleaseEvent(QKeyEvent *ev)
{

}
