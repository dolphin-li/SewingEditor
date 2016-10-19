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
	ldp::Double3 o, u, v;
	int id_l;
	if (getPickedMeshFrameInfo(o, u, v, id_l)){
		m_viewer->beginTrackBall(BaseMeshViewer::TrackBall_Trans, o, 
			ldp::Mat3d().eye(), (u.length() + v.length()) * 0.1);
	}
}

void ClothTranslateEventHandle::handleLeave()
{
	m_viewer->endTrackBall();
}

void ClothTranslateEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mousePressEvent(ev);
	if (m_viewer->pAnalysis()->GetMode() == CLOTH_INITIAL_LOCATION && m_viewer->buttons() == Qt::LeftButton)
	{
		int sid = m_viewer->fboRenderedIndex(ev->pos());
		m_viewer->setHoverTrackBallAxis(sid);
		if (sid >= BaseMeshViewer::TrackBallIndex_X && sid <= BaseMeshViewer::TrackBallIndex_Z)
			m_viewer->setActiveTrackBallAxis(sid);
		else
			pickMesh(ev->pos());
	} // end if initial_location and left button
}

void ClothTranslateEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseReleaseEvent(ev);
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
				m_viewer->beginTrackBall(BaseMeshViewer::TrackBall_Trans, o, 
					ldp::Mat3d().eye(), (u.length() + v.length()) * 0.1);
			}
		} // end else
	} // end if left button and initial_cloth
}

void ClothTranslateEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseDoubleClickEvent(ev);
}

void ClothTranslateEventHandle::mouseMoveEvent(QMouseEvent *ev)
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
		const ldp::Camera& cam = m_viewer->camera();
		ldp::Double3 o, u, v;
		int id_l;
		if (getPickedMeshFrameInfo(o, u, v, id_l))
		{
			QPoint lp = m_viewer->lastMousePos();
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
			ldp::Double3 wp = cam.getWorldCoords(ldp::Float3(ev->x(), m_viewer->height() - 1 - ev->y(), m_picked_screenPos[2]));
			ldp::Double3 wlp = cam.getWorldCoords(ldp::Float3(lp.x(), m_viewer->height() - 1 - lp.y(), m_picked_screenPos[2]));
			ldp::Double3 dir = (wp - wlp)*axis;
			m_viewer->pAnalysis()->MoveClothLoopInitialPosition(id_l, dir.ptr());
			m_viewer->translateTrackBall(dir);
			updateSvg3dInfo();
			valid_op = true;
		} // end if getPickedMeshFrameInfo
	} // end if initial_cloth and left button
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
