#include <QEvent>
#include <GL\glew.h>
#include "BaseMeshViewer.h"

#include "ClothRotateEventHandle.h"
#include "designer2d_cloth.h"
#include "analysis2d_cloth_static.h"
#include "ldpMat\Quaternion.h"

ClothRotateEventHandle::ClothRotateEventHandle(BaseMeshViewer* v) : AbstractMeshEventHandle(v)
{
	m_cursor = QCursor(Qt::CursorShape::SizeAllCursor);
	m_iconFile = "icons/rotation.png";
	m_inactiveIconFile = "icons/rotation_inactive.png";
	m_toolTips = "cloth rotate handle"; 
}

ClothRotateEventHandle::~ClothRotateEventHandle()
{

}

void ClothRotateEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mousePressEvent(ev);
}

void ClothRotateEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseReleaseEvent(ev);
}

void ClothRotateEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseDoubleClickEvent(ev);
}

void ClothRotateEventHandle::mouseMoveEvent(QMouseEvent *ev)
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
			const auto& chandle = m_viewer->pAnalysis()->getClothHandle();
			ldp::Double3 o, n, h;
			chandle.GetAnchor_3D(o.ptr(), n.ptr(), h.ptr(), id_cad_part);
			wp -= o;
			wlp -= o;
			ldp::Mat3d R = ldp::QuaternionD().fromRotationVecs(wlp.normalize(), wp.normalize()).toRotationMatrix3();
			m_viewer->pAnalysis()->RotateClothLoopInitialPosition(id_cad_part, R);
			valid_op = true;
		}
	}
	if (!valid_op)
		AbstractMeshEventHandle::mouseMoveEvent(ev);
}

void ClothRotateEventHandle::wheelEvent(QWheelEvent *ev)
{
	AbstractMeshEventHandle::wheelEvent(ev);
}

void ClothRotateEventHandle::keyPressEvent(QKeyEvent *ev)
{
	AbstractMeshEventHandle::keyPressEvent(ev);
}

void ClothRotateEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractMeshEventHandle::keyReleaseEvent(ev);
}
