#include <QEvent>
#include <GL\glew.h>
#include "BaseMeshViewer.h"

#include "ClothTranslateEventHandle.h"
#include "designer2d_cloth.h"
#include "analysis2d_cloth_static.h"


ClothTranslateEventHandle::ClothTranslateEventHandle(BaseMeshViewer* v) : AbstractMeshEventHandle(v)
{
	m_cursor = QCursor(Qt::CursorShape::SizeAllCursor);
	m_iconFile = "icons/translation.png";
	m_inactiveIconFile = "icons/translation_inactive.png";
	m_toolTips = "cloth handle";
}

ClothTranslateEventHandle::~ClothTranslateEventHandle()
{

}

void ClothTranslateEventHandle::handleEnter()
{

}

void ClothTranslateEventHandle::handleLeave()
{

}

void ClothTranslateEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mousePressEvent(ev);
	// pick a point on the mesh
	if (m_viewer->buttons() == Qt::LeftButton && m_viewer->pAnalysis())
	{
		pickMesh(ev->pos());
	} // end if left button
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
	if (m_viewer->pAnalysis())
	{
		if (m_viewer->pAnalysis()->GetMode() == CLOTH_INITIAL_LOCATION && m_viewer->buttons() == Qt::LeftButton)
		{
			const ldp::Camera& cam = m_viewer->camera();
			ldp::Double3 o, u, v;
			int id_l;
			if (getPickedMeshFrameInfo(o, u, v, id_l))
			{
				QPoint lp = m_viewer->lastMousePos();
				ldp::Double3 wp = cam.getWorldCoords(ldp::Float3(ev->x(), m_viewer->height() - 1 - ev->y(), m_picked_screenPos[2]));
				ldp::Double3 wlp = cam.getWorldCoords(ldp::Float3(lp.x(), m_viewer->height() - 1 - lp.y(), m_picked_screenPos[2]));
				ldp::Double3 dir = wp - wlp;
				m_viewer->pAnalysis()->MoveClothLoopInitialPosition(id_l, dir.ptr());
				updateSvg3dInfo();
				valid_op = true;
			} // end if getPickedMeshFrameInfo
		} // end if left button
	} // end if pAnalysis
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
