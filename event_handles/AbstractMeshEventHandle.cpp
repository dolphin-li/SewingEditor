#include <QEvent>
#include <GL\glew.h>
#include "SewingEditor.h"
#include "BaseMeshViewer.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgAbstractObject.h"

#include "AbstractMeshEventHandle.h"

AbstractMeshEventHandle::AbstractMeshEventHandle(BaseMeshViewer* v)
{
	m_viewer = v;
	m_mainUI = nullptr;
	m_lastHighlightShapeId = -1;
	m_currentSelectedId = -1;
	m_cursor = QCursor(Qt::CursorShape::ArrowCursor);
	m_iconFile = "";
	m_toolTips = "general handle";
}

AbstractMeshEventHandle::~AbstractMeshEventHandle()
{

}

void AbstractMeshEventHandle::setMainUI(MainUI* ui)
{
	m_mainUI = ui;
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
	case AbstractMeshEventHandle::ProcessorTypeEnd:
	default:
		return nullptr;
	}
}

void AbstractMeshEventHandle::mousePressEvent(QMouseEvent *ev)
{
	m_mouse_press_pt = ev->pos();
}

void AbstractMeshEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{

}

void AbstractMeshEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::MouseButton::MiddleButton)
	{
		m_viewer->resetCamera();
	}
}

void AbstractMeshEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	if (m_viewer->buttons() == Qt::NoButton)
	{
	}

	if (m_viewer->buttons() == Qt::MidButton)
	{
	}
}

void AbstractMeshEventHandle::wheelEvent(QWheelEvent *ev)
{
	float s = 1.1;
	if (ev->delta() < 0)
		s = 1.f / s;

	float fov = std::max(1e-3f, std::min(90.f, m_viewer->camera().getFov()*s));
	m_viewer->camera().setPerspective(fov, m_viewer->camera().getAspect(),
		m_viewer->camera().getFrustumNear(), m_viewer->camera().getFrustumFar());
}

void AbstractMeshEventHandle::keyPressEvent(QKeyEvent *ev)
{

}

void AbstractMeshEventHandle::keyReleaseEvent(QKeyEvent *ev)
{

}
