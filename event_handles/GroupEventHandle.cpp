#include "SewingEditor.h"
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
			m_currentSelectedId = id;
			if (ev->modifiers() & Qt::SHIFT)
				op = svg::SvgManager::SelectUnion;
			else if (ev->modifiers() & Qt::CTRL)
				op = svg::SvgManager::SelectCancel;
			m_viewer->getSvgManager()->selectGroupByIndex(id, op);
			if (m_mainUI && id)
				m_mainUI->pushHistory(QString().sprintf("select a group: %d", id));
		}
		else
		{
			if (ev->modifiers() & Qt::SHIFT)
				op = svg::SvgManager::SelectUnion;
			else if (ev->modifiers() & Qt::CTRL)
				op = svg::SvgManager::SelectCancel;
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
			if (m_mainUI && ids.size())
			{
				int id = 0;
				for (auto c : ids)
				if (c)
				{
					id = c;
					break;
				}
				m_currentSelectedId = id;
				m_mainUI->pushHistory(QString().sprintf("select groups: %d, ...", id));
			}
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

	AbstractEventHandle::keyPressEvent(ev);
	svg::SvgManager::SelectOp op = svg::SvgManager::SelectThis;
	switch (ev->key())
	{
	default:
		break;
	case Qt::Key_Plus:
		if (m_currentSelectedId > 0)
		{
			m_currentSelectedId++;
			if (ev->modifiers() & Qt::SHIFT)
				op = svg::SvgManager::SlectionUnionInverse;
			m_viewer->getSvgManager()->selectGroupByIndex(m_currentSelectedId, op);
			if (m_mainUI && m_currentSelectedId)
				m_mainUI->pushHistory(QString().sprintf("select a shape: %d", m_currentSelectedId));
		}
		break;
	case Qt::Key_Minus:
		if (m_currentSelectedId > 0)
		{
			m_currentSelectedId--;
			if (ev->modifiers() & Qt::SHIFT)
				op = svg::SvgManager::SlectionUnionInverse;
			m_viewer->getSvgManager()->selectGroupByIndex(m_currentSelectedId, op);
			if (m_mainUI && m_currentSelectedId)
				m_mainUI->pushHistory(QString().sprintf("select a shape: %d", m_currentSelectedId));
		}
	}

	m_viewer->updateGL();
}

void GroupEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractEventHandle::keyReleaseEvent(ev);
}
