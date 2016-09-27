#include "ZoomEventHandle.h"
#include <QEvent>
#include "SvgViewer.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgAbstractObject.h"
ZoomEventHandle::ZoomEventHandle(SvgViewer* v) :AbstractEventHandle(v)
{
	
}

ZoomEventHandle::~ZoomEventHandle()
{

}

void ZoomEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mousePressEvent(ev);
	m_mouse_press_pt = ev->pos();
	if (m_viewer->buttons() & Qt::LeftButton)
	{
		m_viewer->beginDragBox(ev->pos());
	}
}

void ZoomEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseReleaseEvent(ev);
	if (m_viewer->buttons() & Qt::LeftButton)
	{
		m_viewer->endDragBox();

		float l = m_viewer->camera().getFrustumLeft();
		float r = m_viewer->camera().getFrustumRight();
		float t = m_viewer->camera().getFrustumTop();
		float b = m_viewer->camera().getFrustumBottom();
		float dx = float(ev->pos().x()) / float(m_viewer->width()) * (r - l);
		float dy = float(ev->pos().y()) / float(m_viewer->height()) * (b - t);

		if (ev->pos() == m_mouse_press_pt)
		{
			float s = 1.f / 1.2f;
			r = dx + l + (r - l - dx) * s;
			l = dx + l - dx * s;
			b = dy + t + (b - t - dy) * s;
			t = dy + t - dy * s;
		}
		else
		{
			float x0 = std::min(m_mouse_press_pt.x(), ev->pos().x()) / float(m_viewer->width()) * (r - l) + l;
			float x1 = std::max(m_mouse_press_pt.x(), ev->pos().x()) / float(m_viewer->width()) * (r - l) + l;
			float y0 = std::min(m_mouse_press_pt.y(), ev->pos().y()) / float(m_viewer->height()) * (b - t) + t;
			float y1 = std::max(m_mouse_press_pt.y(), ev->pos().y()) / float(m_viewer->height()) * (b - t) + t;
			float sori = (r - l) / (b - t);
			float snew = (x1 - x0) / (y1 - y0);
			float s = snew / sori;
			if (s < 1)
			{
				l = (x0 + x1) / 2 - (x1 - x0) / s / 2;
				r = (x0 + x1) / 2 + (x1 - x0) / s / 2;
				t = y0;
				b = y1;
			}
			else
			{
				l = x0;
				r = x1;
				t = (y0 + y1) / 2 - (y1 - y0) * s / 2;
				b = (y0 + y1) / 2 + (y1 - y0) * s / 2;
			}
		}

		m_viewer->camera().setFrustum(l, r, t, b,
			m_viewer->camera().getFrustumNear(), m_viewer->camera()
			.getFrustumFar());
	}
}

void ZoomEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseDoubleClickEvent(ev);
}

void ZoomEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	AbstractEventHandle::mouseMoveEvent(ev);
}

void ZoomEventHandle::wheelEvent(QWheelEvent *ev)
{
	AbstractEventHandle::wheelEvent(ev);
}

void ZoomEventHandle::keyPressEvent(QKeyEvent *ev)
{
	AbstractEventHandle::keyPressEvent(ev);
}

void ZoomEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractEventHandle::keyReleaseEvent(ev);
}
