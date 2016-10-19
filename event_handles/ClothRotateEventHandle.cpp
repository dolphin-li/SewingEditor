#include <QEvent>
#include <GL\glew.h>
#include "BaseMeshViewer.h"

#include "ClothRotateEventHandle.h"
#include "designer2d_cloth.h"
#include "analysis2d_cloth_static.h"
#include "ldpMat\Quaternion.h"

ClothRotateEventHandle::ClothRotateEventHandle(BaseMeshViewer* v)
: AbstractMeshEventHandle(v)
{
	m_cursor = QCursor(Qt::CursorShape::CrossCursor);
	m_iconFile = "icons/rotation.png";
	m_inactiveIconFile = "icons/rotation_inactive.png";
	m_toolTips = "cloth rotate handle"; 
}

ClothRotateEventHandle::~ClothRotateEventHandle()
{

}

void ClothRotateEventHandle::handleEnter()
{
	AbstractMeshEventHandle::handleEnter();
	ldp::Double3 o, u, v;
	int id_l;
	if (getPickedMeshFrameInfo(o, u, v, id_l)){
		ldp::Mat3d R = rotation_from_uv(u, v);
		m_trackBallMouseClickR = R;
		m_viewer->beginTrackBall(BaseMeshViewer::TrackBall_Rot, o, R, (u.length() + v.length()) * 0.1);
	}
}

void ClothRotateEventHandle::handleLeave()
{
	m_viewer->endTrackBall();
	AbstractMeshEventHandle::handleLeave();
}

void ClothRotateEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mousePressEvent(ev);

	if (!m_viewer->pAnalysis()) return;

	if (m_viewer->pAnalysis()->GetMode() == CLOTH_INITIAL_LOCATION && m_viewer->buttons() == Qt::LeftButton)
	{
		int sid = m_viewer->fboRenderedIndex(ev->pos());
		m_viewer->setHoverTrackBallAxis(sid);
		if (sid >= BaseMeshViewer::TrackBallIndex_X && sid <= BaseMeshViewer::TrackBallIndex_Z)
		{
			m_viewer->setActiveTrackBallAxis(sid);
			ldp::Double3 o, u, v;
			int id_l;
			if (getPickedMeshFrameInfo(o, u, v, id_l)){
				ldp::Mat3d R = rotation_from_uv(u, v);
				m_trackBallMouseClickR = R;
			}
		} // end if trackBallAxisMode
		else
			pickMesh(ev->pos());
	} // end if initial_location and left button
}

void ClothRotateEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	if (!m_viewer->pAnalysis()) return;

	if (m_viewer->pAnalysis()->GetMode() == CLOTH_INITIAL_LOCATION && m_viewer->buttons() == Qt::LeftButton)
	{
		if (m_viewer->getActiveTrackBallAxis() > 0)
		{
			m_viewer->setActiveTrackBallAxis(-1);
		} // end if track ball axis active
		else
		{
			ldp::Double3 o, u, v;
			int id_l;
			if (getPickedMeshFrameInfo(o, u, v, id_l)){
				ldp::Mat3d R = rotation_from_uv(u, v);
				m_trackBallMouseClickR = R;
				m_viewer->beginTrackBall(BaseMeshViewer::TrackBall_Rot, o, R, (u.length() + v.length()) * 0.1);
			}
		} // end else
	} // end if left button and initial_cloth

	AbstractMeshEventHandle::mouseReleaseEvent(ev);
}

void ClothRotateEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseDoubleClickEvent(ev);
}

void ClothRotateEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	bool valid_op = false;

	int sid = m_viewer->getActiveTrackBallAxis();
	if (sid < BaseMeshViewer::TrackBallIndex_X || sid > BaseMeshViewer::TrackBallIndex_Z)
	{
		int hid = m_viewer->fboRenderedIndex(ev->pos());
		if (hid >= BaseMeshViewer::TrackBallIndex_X && hid <= BaseMeshViewer::TrackBallIndex_Z)
			m_viewer->setHoverTrackBallAxis(hid);
		else
			m_viewer->setHoverTrackBallAxis(0);
	}
	else if (m_viewer->pAnalysis()->GetMode() == CLOTH_INITIAL_LOCATION && m_viewer->buttons() == Qt::LeftButton)
	{
		ldp::Double3 c, u, v;
		int id_l;
		if (getPickedMeshFrameInfo(c, u, v, id_l))
		{
			QPoint lp = m_mouse_press_pt;
			ldp::Float3 axis = 0;
			switch (sid)
			{
			default:
				break;
			case (int)BaseMeshViewer::TrackBallIndex_X:
				axis[0] = 1;
				break;
			case (int)BaseMeshViewer::TrackBallIndex_Y:
				axis[1] = 1;
				break;
			case (int)BaseMeshViewer::TrackBallIndex_Z:
				axis[2] = 1;
				break;
			}
			ldp::Mat3d lastR = rotation_from_uv(u, v);
			axis = m_trackBallMouseClickR * axis;
			ldp::Float3 c1 = c + axis;
			ldp::Float3 c_uvd = m_viewer->camera().getScreenCoords(c);
			ldp::Float3 c1_uvd = m_viewer->camera().getScreenCoords(c1);
			ldp::Float2 c_uv(c_uvd[0] / c_uvd[2], c_uvd[1] / c_uvd[2]);
			c_uv[1] = m_viewer->camera().getViewPortBottom() - c_uv[1];
			ldp::Float2 d1 = (ldp::Float2(ev->x(), ev->y()) - c_uv).normalize();
			ldp::Float2 d2 = (ldp::Float2(lp.x(), lp.y()) - c_uv).normalize();
			float ag = atan2(d1.cross(d2), d1.dot(d2));
			if (c_uvd[2] < c1_uvd[2]) ag = -ag;
			auto R = ldp::QuaternionF().fromAngleAxis(ag, axis).toRotationMatrix3() * m_trackBallMouseClickR;

			m_viewer->pAnalysis()->RotateClothLoopInitialPosition(id_l, R*lastR.trans());
			m_viewer->rotateTrackBall(R*lastR.trans());
			updateSvg3dInfo();
			valid_op = true;
		}
	} // end if initial_cloth and left button
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
