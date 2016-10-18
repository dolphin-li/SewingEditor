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
	bool valid_op = false;
	if (m_viewer->pAnalysis()->GetMode() == CLOTH_INITIAL_LOCATION && m_viewer->buttons() == Qt::LeftButton)
	{
		const ldp::Camera& cam = m_viewer->camera();
		Cad::CAD_ELEM_TYPE itype_cad_part; unsigned int id_cad_part;
		double tmp_x, tmp_y;
		m_viewer->pListener()->Cad_GetPicked(itype_cad_part, id_cad_part, tmp_x, tmp_y);
		if (itype_cad_part == Cad::LOOP && m_viewer->pListener()->GetCad().IsElemID(itype_cad_part, id_cad_part))
		{
			QPoint lp = m_viewer->lastMousePos();
			ldp::Double3 wp = cam.getWorldCoords(ldp::Float3(ev->x(), m_viewer->height() - 1 - ev->y(), m_picked_screenPos[2]));
			ldp::Double3 wlp = cam.getWorldCoords(ldp::Float3(lp.x(), m_viewer->height() - 1 - lp.y(), m_picked_screenPos[2]));
			ldp::Double3 dir = wp - wlp;
			m_viewer->pAnalysis()->MoveClothLoopInitialPosition(id_cad_part, dir.ptr());
			valid_op = true;
		}
	}
	if (!valid_op)
		AbstractMeshEventHandle::mouseMoveEvent(ev);
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
