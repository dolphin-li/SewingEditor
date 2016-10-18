#include <QEvent>
#include <GL\glew.h>
#include "BaseMeshViewer.h"

#include "ClothTranslateEventHandle.h"
#include "designer2d_cloth.h"
#include "analysis2d_cloth_static.h"

ClothTranslateEventHandle::ClothTranslateEventHandle(BaseMeshViewer* v) : AbstractMeshEventHandle(v)
{
	m_cursor = QCursor(Qt::CursorShape::CrossCursor);
	m_iconFile = "icons/translation.png";
	m_inactiveIconFile = "icons/translation_inactive.png";
	m_toolTips = "cloth handle";
}

ClothTranslateEventHandle::~ClothTranslateEventHandle()
{

}

void ClothTranslateEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mousePressEvent(ev);

	if (m_viewer->buttons() == Qt::LeftButton)
	{
		const ldp::Camera& cam = m_viewer->camera();
		ldp::Double3 trans0 = cam.getModelViewMatrix().getTranslationPart();
		ldp::Mat3d R = cam.getModelViewMatrix().getRotationPart();
		ldp::Double3 trans1 = 0;
		const float vl = cam.getViewPortLeft();
		const float vr = cam.getViewPortRight();
		const float vt = cam.getViewPortTop();
		const float vb = cam.getViewPortBottom();
		const float vx = (ev->x() - vl) / (vr - vl) * 2 - 1;
		const float vy = (ev->y() - vt) / (vb - vt) * 2 - 1;
		const float frustHalfH = (cam.getFrustumTop() - cam.getFrustumBottom()) / 2.f;
		const float frustHalfW = (cam.getFrustumRight() - cam.getFrustumLeft()) / 2.f;
		unsigned int no[3], id_l;
		double r[3];
		bool res = m_viewer->pAnalysis()->Pick(frustHalfW*vx, frustHalfH*vy,
			trans0.ptr(), R.trans().ptr(), trans1.ptr(), no, r, id_l);//, dir,org); 
		if (!res)
		{
			m_viewer->pListener()->HilightCadTypeID(Cad::NOT_SET, 0);
			m_viewer->pListener()->Cad_SetPicked(Cad::NOT_SET, 0, 0, 0);
			return;
		}
		m_viewer->pListener()->HilightCadTypeID(Cad::LOOP, id_l);
		m_viewer->pListener()->Cad_SetPicked(Cad::LOOP, id_l, 0, 0);
	}
}

void ClothTranslateEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseReleaseEvent(ev);
}

void ClothTranslateEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseDoubleClickEvent(ev);
}

void ClothTranslateEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseMoveEvent(ev);

	if (m_viewer->pAnalysis()->GetMode() == CLOTH_INITIAL_LOCATION && m_viewer->buttons() == Qt::LeftButton)
	{
		const ldp::Camera& cam = m_viewer->camera();
		Cad::CAD_ELEM_TYPE itype_cad_part; unsigned int id_cad_part;
		double tmp_x, tmp_y;
		m_viewer->pListener()->Cad_GetPicked(itype_cad_part, id_cad_part, tmp_x, tmp_y);
		if (itype_cad_part == Cad::LOOP && m_viewer->pListener()->GetCad().IsElemID(itype_cad_part, id_cad_part))
		{
			double dir[3];
			{
				const float hvh = (cam.getViewPortBottom() - cam.getViewPortTop()) / 2.f / cam.getScalar()[0];
				const float asp = cam.getAspect();
				QPoint lp = m_viewer->lastMousePos();
				ldp::Float3 dir((ev->x() - lp.x())*hvh*asp, (ev->y() - lp.y())*hvh, 0);
				ldp::Mat3f R = cam.getModelViewMatrix().getRotationPart();
				dir = R * dir;
			}
			m_viewer->pAnalysis()->MoveClothLoopInitialPosition(id_cad_part, dir);
		}
	}
}

void ClothTranslateEventHandle::wheelEvent(QWheelEvent *ev)
{
	AbstractMeshEventHandle::wheelEvent(ev);
}

void ClothTranslateEventHandle::keyPressEvent(QKeyEvent *ev)
{
	AbstractMeshEventHandle::keyPressEvent(ev);
}

void ClothTranslateEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractMeshEventHandle::keyReleaseEvent(ev);
}
