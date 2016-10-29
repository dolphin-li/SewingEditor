#include "SewingEditor.h"
#include "DeformEventHandle.h"
#include <QEvent>
#include "SvgViewer.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgAbstractObject.h"
#include "FreeFormDeform.h"
DeformEventHandle::DeformEventHandle(SvgViewer* v) :AbstractEventHandle(v)
{
	m_cursor = QCursor(Qt::CursorShape::CrossCursor);
	m_iconFile = "icons/deformArrow.png";
	m_toolTips = "deformation";
}

DeformEventHandle::~DeformEventHandle()
{

}

void DeformEventHandle::handleEnter()
{
	auto svg = m_viewer->getSvgManager();
	auto deformer = m_viewer->getSvgDeformer();
	if (svg && deformer)
	{
		auto objs = svg->collectPolyPaths();
		deformer->init(objs);
		m_viewer->beginDeformMode();
		m_viewer->updateGL();
	} // end if svg
}

void DeformEventHandle::handleLeave()
{
	auto svg = m_viewer->getSvgManager();
	auto deformer = m_viewer->getSvgDeformer();
	if (svg && deformer)
	{
		svg->updateBound();
		deformer->clear();
	} // end if svg
	m_viewer->endDeformMode();
	m_viewer->updateGL();
}

void DeformEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mousePressEvent(ev);
	auto svg = m_viewer->getSvgManager();
	auto deformer = m_viewer->getSvgDeformer();
	if (!svg || !deformer)
		return;
	if (m_viewer->fboImage().rect().contains(ev->pos()))
	{
		QRgb cl = m_viewer->fboImage().pixel(ev->pos());
		ldp::Float4 color(qRed(cl), qGreen(cl), qBlue(cl), qAlpha(cl));
		color /= 255.f;
		int id = svg::SvgAbstractObject::index_from_color(color) - FreeFormDeform::INDEX_BEGIN;
		if (id >= 0 && id < deformer->numControlPoints())
			m_viewer->setActiveDeformNodeId(id);
	}
	if (m_viewer->buttons() & Qt::LeftButton && m_viewer->getActiveDeformNodeId() < 0)
	{
		if (ev->modifiers() == Qt::CTRL)
		{
			ldp::Float2 p(ev->x(), ev->y());
			p = m_viewer->viewCoord2svgCoord(p);
			auto idx = deformer->addControlPoint(p);
			if (idx >= 0)
			{
				m_viewer->setActiveDeformNodeId(idx);
				if (m_mainUI)
					m_mainUI->pushHistory("add control points");
			} // end if idx
		} // end if control
		m_viewer->setActiveDeformNodeId(-1);
	} // end if left button
}

void DeformEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseReleaseEvent(ev);
	auto svg = m_viewer->getSvgManager();
	auto deformer = m_viewer->getSvgDeformer();
	if (!svg || !deformer)
		return;
	int aid = m_viewer->getActiveDeformNodeId();
	if (m_viewer->buttons() == Qt::LeftButton && aid >= 0 && aid < deformer->numControlPoints())
	{
		if (ev->pos() != m_mouse_press_pt)
		{
			deformer->setControlPointTargetPos(aid, m_viewer->viewCoord2svgCoord(ldp::Float2(ev->x(), ev->y())));
			if (m_mainUI)
				m_mainUI->pushHistory("deform drag");
		}
	}
	m_viewer->setActiveDeformNodeId(-1);
}

void DeformEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseDoubleClickEvent(ev);
}

void DeformEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseMoveEvent(ev);

	auto svg = m_viewer->getSvgManager();
	auto deformer = m_viewer->getSvgDeformer();
	if (!svg || !deformer)
		return;
	if (m_viewer->fboImage().rect().contains(ev->pos()))
	{
		QRgb cl = m_viewer->fboImage().pixel(ev->pos());
		ldp::Float4 color(qRed(cl), qGreen(cl), qBlue(cl), qAlpha(cl));
		color /= 255.f;
		int id = svg::SvgAbstractObject::index_from_color(color) - FreeFormDeform::INDEX_BEGIN;
		if (m_viewer->getActiveDeformNodeId() < 0)
		{
			if (id >= 0 && id < deformer->numControlPoints())
				m_viewer->setHighLightedDeformNodeId(id);
			else
				m_viewer->setHighLightedDeformNodeId(-1);
		}
	}

	if (m_viewer->buttons() == Qt::LeftButton)
	{
		int aid = m_viewer->getActiveDeformNodeId();
		if (aid >= 0 && aid < deformer->numControlPoints())
		{
			deformer->setControlPointTargetPos(aid, m_viewer->viewCoord2svgCoord(ldp::Float2(ev->x(), ev->y())));
		}
	}
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
