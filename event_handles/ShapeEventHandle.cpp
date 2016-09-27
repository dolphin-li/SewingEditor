#include "ShapeEventHandle.h"
#include <QEvent>
#include "SvgViewer.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgAbstractObject.h"
ShapeEventHandle::ShapeEventHandle(SvgViewer* v) :AbstractEventHandle(v)
{
	
}

ShapeEventHandle::~ShapeEventHandle()
{

}

void ShapeEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mousePressEvent(ev);
}

void ShapeEventHandle::mouseReleaseEvent(QMouseEvent *ev)
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
		m_viewer->getSvgManager()->selectShapeByIndex(id, op);
	}
}

void ShapeEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseDoubleClickEvent(ev);
}

void ShapeEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseMoveEvent(ev);
}

void ShapeEventHandle::wheelEvent(QWheelEvent *ev)
{
	AbstractEventHandle::wheelEvent(ev);
}

void ShapeEventHandle::keyPressEvent(QKeyEvent *ev)
{
	AbstractEventHandle::keyPressEvent(ev);
}

void ShapeEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractEventHandle::keyReleaseEvent(ev);
}
