#include "GroupEventHandle.h"
#include <QEvent>
#include "SvgViewer.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgAbstractObject.h"
GroupEventHandle::GroupEventHandle(SvgViewer* v) :AbstractEventHandle(v)
{
	
}

GroupEventHandle::~GroupEventHandle()
{

}

void GroupEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mousePressEvent(ev);
}

void GroupEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseReleaseEvent(ev);
	if (m_viewer->buttons() & Qt::LeftButton)
	{
		QRgb cl = m_viewer->fboImage().pixel(ev->pos());
		ldp::Float4 color(qRed(cl), qGreen(cl), qBlue(cl), qAlpha(cl));
		color /= 255.f;
		int id = svg::SvgAbstractObject::index_from_color(color);
		svg::SvgManager::SelectOp op = svg::SvgManager::SelectThis;
		if (ev->modifiers() & Qt::SHIFT)
			op = svg::SvgManager::SelectUnion;
		m_viewer->getSvgManager()->selectGroupByIndex(id, op);
	}
}

void GroupEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseDoubleClickEvent(ev);
}

void GroupEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseMoveEvent(ev);
}

void GroupEventHandle::wheelEvent(QWheelEvent *ev)
{
	AbstractEventHandle::wheelEvent(ev);
}

void GroupEventHandle::keyPressEvent(QKeyEvent *ev)
{
	AbstractEventHandle::keyPressEvent(ev);
}

void GroupEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractEventHandle::keyReleaseEvent(ev);
}
