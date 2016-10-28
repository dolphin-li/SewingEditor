#include "DeformEventHandle.h"
#include <QEvent>
#include "SvgViewer.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgAbstractObject.h"
DeformEventHandle::DeformEventHandle(SvgViewer* v) :AbstractEventHandle(v)
{
	m_cursor = QCursor(Qt::CursorShape::CrossCursor);
	m_iconFile = "icons/deformArrow.png";
	m_toolTips = "deformation";
}

DeformEventHandle::~DeformEventHandle()
{

}

void DeformEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mousePressEvent(ev);
	if (m_viewer->buttons() & Qt::LeftButton)
	{
		
	}
}

void DeformEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseReleaseEvent(ev);
	if (m_viewer->buttons() & Qt::LeftButton)
	{
		
	}
}

void DeformEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseDoubleClickEvent(ev);
}

void DeformEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseMoveEvent(ev);
}

void DeformEventHandle::wheelEvent(QWheelEvent *ev)
{
	AbstractEventHandle::wheelEvent(ev);
}

void DeformEventHandle::keyPressEvent(QKeyEvent *ev)
{
	AbstractEventHandle::keyPressEvent(ev);
}

void DeformEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractEventHandle::keyReleaseEvent(ev);
}
