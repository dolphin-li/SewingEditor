#include "GroupEventHandle.h"
#include <QEvent>
#include "SvgViewer.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgAbstractObject.h"
GroupEventHandle::GroupEventHandle(SvgViewer* v) :AbstractEventHandle(v)
{
	m_cursor = QCursor(Qt::CursorShape::UpArrowCursor);
	QFileInfo info = QFileInfo("icons/groupCursor.png");
	if (info.exists())
		m_cursor = QCursor(QPixmap(info.absoluteFilePath()), 4, 1);
	m_iconFile = "icons/groupArrow.png";
	m_toolTips = "group selection";
}

GroupEventHandle::~GroupEventHandle()
{

}

void GroupEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mousePressEvent(ev);
	if (m_viewer->buttons() & Qt::LeftButton)
	{
		m_viewer->beginDragBox(ev->pos());
	}
}

void GroupEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseReleaseEvent(ev);
	if (m_viewer->buttons() & Qt::LeftButton)
	{
		svg::SvgManager::SelectOp op = svg::SvgManager::SelectThis;
		if (ev->pos() == m_mouse_press_pt)
		{
			QRgb cl = m_viewer->fboImage().pixel(ev->pos());
			ldp::Float4 color(qRed(cl), qGreen(cl), qBlue(cl), qAlpha(cl));
			color /= 255.f;
			int id = svg::SvgAbstractObject::index_from_color(color);
			if (ev->modifiers() & Qt::SHIFT)
				op = svg::SvgManager::SlectionUnionInverse;
			m_viewer->getSvgManager()->selectGroupByIndex(id, op);
		}
		else
		{
			if (ev->modifiers() & Qt::SHIFT)
				op = svg::SvgManager::SelectUnion;
			const QImage& I = m_viewer->fboImage();
			std::set<int> ids;
			float x0 = std::max(0, std::min(m_mouse_press_pt.x(), ev->pos().x()));
			float x1 = std::min(I.width() - 1, std::max(m_mouse_press_pt.x(), ev->pos().x()));
			float y0 = std::max(0, std::min(m_mouse_press_pt.y(), ev->pos().y()));
			float y1 = std::min(I.height() - 1, std::max(m_mouse_press_pt.y(), ev->pos().y()));
			for (int y = y0; y <= y1; y++)
			for (int x = x0; x <= x1; x++)
			{
				QRgb cl = m_viewer->fboImage().pixel(x, y);
				ldp::Float4 color(qRed(cl), qGreen(cl), qBlue(cl), qAlpha(cl));
				color /= 255.f;
				int id = svg::SvgAbstractObject::index_from_color(color);
				ids.insert(id);
			}
			m_viewer->getSvgManager()->selectGroupByIndex(ids, op);
		}
	}
	m_viewer->endDragBox();
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
